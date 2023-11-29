# TS3 Server Hook

Dependencies:
- [libmem] (using prebuilt binaries from [nathan818fr/libmem-build][libmem-build])
- [asio]

[libmem]: https://github.com/rdbo/libmem
[libmem-build]: https://github.com/nathan818fr/libmem-build
[asio]: https://github.com/chriskohlhoff/asio

Build from source:
```bash
git clone --recurse-submodules https://github.com/nathan818fr/ts3-server-hook.git
cd ts3-server-hook
./tools/fetch-libmem.sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel "$(nproc)"
```
