#!/usr/bin/env sh
commit=$(git describe --always --dirty=~)
tag=$(git describe --tags --always --dirty=~)

if [ -z "$commit" ]; then
  commit='unknown'
fi
if [ -z "$tag" ] || [ "$tag" = "$commit" ]; then
  tag='unknown'
fi

printf "%s (%s)\n" "${tag#v}" "$commit"
exit 0
