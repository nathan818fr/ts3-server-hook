#!/usr/bin/env sh
# Dependencies:
# - cpplint (https://github.com/cpplint/cpplint / `pipx install cpplint`)
cd -- "$(dirname -- "$(realpath -m -- "$0")")/.." || exit 1
exec find ./src \( -name '*.cc' -o -name '*.h' \) -exec cpplint \
  --quiet \
  --linelength=120 \
  --filter=-legal,-build/c++11,-build/include_subdir,-readability/nolint,-whitespace/indent,-whitespace/parens \
  {} +
