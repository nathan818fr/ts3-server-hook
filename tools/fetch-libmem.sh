#!/usr/bin/env bash
set -Eeuo pipefail
shopt -s inherit_errexit

declare -r LIBMEM_VERSION='4.2.1'

function main() {
  local script_dir libmem_dir libmem_platform libmem_url
  script_dir=$(dirname -- "$(realpath -m -- "$0")")
  libmem_dir="$(dirname -- "$script_dir")/external/libmem"
  libmem_platform='linux-gnu-x86_64' # TODO: auto-detect
  libmem_url="https://github.com/nathan818fr/libmem-build/releases/download/v${LIBMEM_VERSION}/libmem-${LIBMEM_VERSION}-${libmem_platform}.tar.gz"

  printf "libmem directory: %s\n" "$libmem_dir"
  printf "libmem platform: %s\n" "$libmem_platform"
  printf "libmem URL: %s\n" "$libmem_url"
  printf '\n'

  if [[ "$(cat "$libmem_dir/.source" 2>/dev/null || true)" == "$libmem_url" ]]; then
    printf 'libmem is already up-to-date!\n'
    return
  fi

  rm -rf -- "$libmem_dir"
  mkdir -p -- "$libmem_dir"
  curl -fL -- "$libmem_url" | tar -xz -C "$libmem_dir" --strip-components=1
  printf "%s\n" "$libmem_url" >"${libmem_dir}/.source"
  printf 'libmem downloaded!\n'
}

eval 'main "$@";exit "$?"'
