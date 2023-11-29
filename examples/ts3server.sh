#!/usr/bin/env bash
set -Eeuo pipefail
shopt -s inherit_errexit

function main() {
  ts3server_dir="$(dirname -- "$(realpath -m -- "$0")")/ts3server"
  declare -gr ts3server_dir

  case "${1:-}" in
  install)
    cmd_install
    ;;
  run)
    shift
    cmd_run "$@"
    ;;
  start | stop | restart | status)
    ts3server "$@"
    ;;
  *)
    printf 'Usage: %s {install|run|start|stop|restart|status}\n' "$(basename -- "$0")" >&2
    return 1
    ;;
  esac
}

function cmd_install() {
  if [[ -d "$ts3server_dir" ]]; then
    printf 'error: ts3server directory already exists: %s\n' "$ts3server_dir" >&2
    printf 'hint: Delete it and try again to reinstall.\n' >&2
    return 1
  fi

  local ts3server_url
  if [[ -n "${TS3SERVER_URL:-}" ]]; then
    ts3server_url="$TS3SERVER_URL"
  else
    ts3server_url="https://files.teamspeak-services.com/releases/server/3.13.7/teamspeak3-server_linux_amd64-3.13.7.tar.bz2"
  fi
  printf 'TS3 Server URL: %s\n' "$ts3server_url" >&2

  mkdir -p -- "$ts3server_dir"
  curl -fL -- "https://files.teamspeak-services.com/releases/server/3.13.7/teamspeak3-server_linux_amd64-3.13.7.tar.bz2" |
    tar -xj -C "$ts3server_dir" --strip-components=1
  printf 'TS3 Server installed to: %s\n' "$ts3server_dir" >&2
}

function cmd_run() {
  pushd -- "$ts3server_dir" >/dev/null
  exec_with_hook ./ts3server "$@"
}

function ts3server() {
  pushd -- "$ts3server_dir" >/dev/null
  exec_with_hook ./ts3server_startscript.sh "$@"
}

function exec_with_hook() {
  local ts3_server_hook
  ts3_server_hook=$(find_hook)
  printf 'Using ts3-server-hook.so: %s\n' "$ts3_server_hook" >&2

  set -x
  LD_PRELOAD="${ts3_server_hook}:${LD_PRELOAD:-}" exec "$@"
}

function find_hook() {
  local project_dir
  project_dir="$(dirname -- "$(dirname -- "$ts3server_dir")")"

  local ts3_server_hook build_dir
  for build_dir in build cmake-build-debug cmake-build-release; do
    ts3_server_hook="${project_dir}/${build_dir}/ts3-server-hook.so"
    if [[ -f "$ts3_server_hook" ]]; then
      printf '%s' "$ts3_server_hook"
      return
    fi
  done

  printf 'error: ts3-server-hook.so not found\n' >&2
  printf 'hint: Build the project with CMake first.\n' >&2
  return 1
}

eval 'main "$@";exit "$?"'
