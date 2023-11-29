#!/usr/bin/env sh
# Dependencies:
# - clang-format (>= 16)
cd -- "$(dirname -- "$(realpath -m -- "$0")")/.." || exit 1
exec find ./src \( -name '*.cc' -o -name '*.h' \) -exec clang-format --verbose -i {} +
