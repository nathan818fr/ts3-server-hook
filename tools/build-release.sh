#!/usr/bin/env bash
set -Eeuo pipefail
shopt -s inherit_errexit

declare -gr PLATFORMS=(
  linux-gnu-x86_64
)

SCRIPT_DIR=$(dirname -- "$(realpath -m -- "$0")")
declare -gr SCRIPT_DIR
REPO_DIR=$(dirname -- "$SCRIPT_DIR")
declare -gr REPO_DIR

function print_usage() {
  cat <<EOF
Usage: $(basename -- "$0") <platform>

Supported platforms:
$(printf '  - %s\n' "${PLATFORMS[@]}")
EOF
}

function main() {
  if [[ $# -ne 1 ]]; then
    print_usage >&2
    return 1
  fi

  local platform=$1
  if ! array_contains "$platform" "${PLATFORMS[@]}"; then
    printf 'error: Unknown platform: %s\n' "$platform" >&2
    return 1
  fi

  local source_dir=${REPO_DIR}
  local out_dir
  if [[ -n "${TS3SH_BUILD_OUT_DIR:-}" ]]; then
    out_dir=$(realpath -m -- "$TS3SH_BUILD_OUT_DIR")
    if [[ -d "$out_dir" ]]; then
      printf 'error: Output directory already exists: %s\n' "$out_dir" >&2
      return 1
    fi
  else
    out_dir=$(realpath -m -- "build/out/ts3-server-hook-local-${platform}")
    rm -rf -- "$out_dir"
  fi
  mkdir -p -- "$out_dir"

  printf 'Platform: %s\n' "$platform"
  printf 'Source directory: %s\n' "$source_dir"
  printf 'Output directory: %s\n' "$out_dir"
  printf '\n'

  case "$platform" in
  linux-*) _build_in_docker "$platform" "$source_dir" "$out_dir" ;;
  *) _build_locally "$platform" "$source_dir" "$out_dir" ;;
  esac

  if [[ "${TS3SH_BUILD_SKIP_ARCHIVE:-}" != true ]]; then
    printf '[+] Create archive\n'
    tar -czf "${out_dir}.tar.gz" --owner=0 --group=0 --numeric-owner -C "$(dirname -- "$out_dir")" "$(basename -- "$out_dir")"
  fi

  printf '[+] Done\n'
}

function _build_in_docker() {
  local platform=$1 source_dir=$2 out_dir=$3

  local docker_os=unknown docker_platform=unknown
  case "$platform" in
  linux-gnu-*) docker_os=linux-gnu ;;
  esac
  case "$platform" in
  *-x86_64) docker_platform=linux/amd64 ;;
  esac

  local docker_image="ts3sh-build-${docker_os}-${docker_platform##*/}"
  docker build --platform "$docker_platform" -t "$docker_image" -f "${SCRIPT_DIR}/docker-env/${docker_os}.Dockerfile" "${SCRIPT_DIR}/docker-env"
  docker run --platform "$docker_platform" --rm \
    -e "PUID=$(id -u)" \
    -e "PGID=$(id -g)" \
    -e "_PLATFORM=${platform}" \
    -e "_SOURCE_DIR=/source" \
    -e "_BUILD_DIR=/build" \
    -e "_OUT_DIR=/out" \
    -v "${source_dir}:/source:ro" \
    -v "${out_dir}:/out:rw" \
    -i "$docker_image" \
    bash <<<"set -Eeuo pipefail; shopt -s inherit_errexit; $(declare -f do_build); do_build; exit 0"
}

function _build_locally() {
  local platform=$1 source_dir=$2 out_dir=$3

  local local_env="${platform%-*}"
  local local_env_arch="${platform##*-}"

  init_temp_dir
  _=_ \
    _PLATFORM="$platform" \
    _SOURCE_DIR="$source_dir" \
    _BUILD_DIR="${g_temp_dir}/build" \
    _OUT_DIR="$out_dir" \
    "${REPO_DIR}/local-env/${local_env}.sh" "$local_env_arch" \
    bash <<<"set -Eeuo pipefail; shopt -s inherit_errexit; $(declare -f do_build); do_build; exit 0"
}

# Perform the build and copy the results to the output directory.
# This function must self-contained and exportable.
# Inputs:
#   _PLATFORM: The target platform.
#   _SOURCE_DIR: The absolute path to the source directory.
#   _BUILD_DIR: The absolute path to the build directory.
#   _OUT_DIR: The absolute path to the output directory.
function do_build() {
  true "${_PLATFORM?required}" "${_SOURCE_DIR?required}" "${_BUILD_DIR?required}" "${_OUT_DIR?required}"

  set -x
  cmake -S "$_SOURCE_DIR" -B "$_BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
  cmake --build "$_BUILD_DIR" --config Release --parallel "$(nproc)"
  { set +x; } 2>/dev/null

  # Copy binary
  printf '[+] Copy binary\n'
  install -vD -m644 -- "${_BUILD_DIR}/ts3-server-hook.so" "${_OUT_DIR}/ts3-server-hook.so"

  # Copy information files
  printf '[+] Copy information files\n'
  install -vD -m644 -- "${_SOURCE_DIR}/README.md" "${_OUT_DIR}/README.md"
  install -vD -m644 -- "${_SOURCE_DIR}/LICENSE.md" "${_OUT_DIR}/LICENSE.md"
  install -vD -m644 -- "${_SOURCE_DIR}/LICENSE_EXCEPTIONS.md" "${_OUT_DIR}/LICENSE_EXCEPTIONS.md"

  # Add stdlib information (glibc version, etc)
  printf '[+] Add stdlib information\n'
  case "$_PLATFORM" in
  linux-gnu-*)
    # ldd --version can cause exit 141 when stdout is closed early.
    { ldd --version || true; } | head -n1 | awk '{print $NF}' | install -vD -m644 -- /dev/stdin "${_OUT_DIR}/GLIBC_VERSION.txt"
    ;;
  esac
}

# Ensure that the temporary directory exists and is set to an absolute path.
# This directory will be automatically deleted when the script exits.
# This function is idempotent.
# Outputs:
#   g_temp_dir: The absolute path to the temporary directory.
function init_temp_dir() {
  if [[ -v g_temp_dir ]]; then
    return
  fi

  g_temp_dir=$(mktemp -d)
  declare -gr g_temp_dir

  # shellcheck disable=SC2317
  function __temp_dir_cleanup() { rm -rf -- "$g_temp_dir"; }
  trap __temp_dir_cleanup INT TERM EXIT
}

# Check if an array contains a value.
# Inputs:
#   $1: The value to check for.
#   $2+: The array to check.
# Returns:
#   0 if the array contains the value,
#   1 otherwise.
function array_contains() {
  local value=$1
  local element
  for element in "${@:2}"; do
    if [[ "$element" == "$value" ]]; then
      return 0
    fi
  done
  return 1
}

eval 'main "$@";exit "$?"'
