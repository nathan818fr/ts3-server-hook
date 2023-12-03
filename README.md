# TS3 Server Hook

TS3 Server Hook is a mod for TeamSpeak 3 servers that intercepts data received
by the server (messages, voice, etc.) and forward them through a socket.

## Features

- Capture commands packets (channel messages, private messages, pokes, etc.)
- Capture voice packets

## Compatibility

<table>
<thead>
  <tr>
    <th>Platform</th>
    <th>Architecture</th>
    <th>Supported server version</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td rowspan=2>Linux (glibc)</td>
    <td>x86_64 <i>(64-bit)</i></td>
    <td>✅ 3.13.6 | 3.13.7</td>
  </tr>
  <tr>
    <td>i686 <i>(32-bit)</i></td>
    <td rowspan=5>❌ Not supported</td>
  </tr>
  <tr>
    <td colspan=2>Linux (alpine)</td>
  </tr>
  <tr>
    <td colspan=2>FreeBSD</td>
  </tr>
  <tr>
    <td colspan=2>Windows</td>
  </tr>
  <tr>
    <td colspan=2>macOS</td>
  </tr>
</tbody>
</table>

*Support for platforms other than Linux 64-bit is not planned.*<br>
*But feel free to open an issue if you really want to see support for another
platform.*

## Installation

Download the latest release from the [releases page].
Make sure to download the correct file for your platform and TeamSpeak server
version.

[releases page]: https://github.com/nathan818fr/ts3-server-hook/releases

Extract the archive and copy `ts3-server-hook.so` to the location of your
choice.

Then run the following command to start your TeamSpeak server:
```sh
LD_PRELOAD=/path/to/ts3-server-hook.so ./ts3server_startscript.sh start
```

A `ts3-server-hook.sock` file should appear in your TeamSpeak server directory.
This is the socket that the mod use to forward data.

## Usage

If you are a developer, see the [protocol documentation](PROTOCOL.md).

If you are not a developer, you should look for a software that uses this mod,
like:
- *Nothing yet*

## Development

### Build

This project uses CMake to build.
You should clone the repository with submodules and run
`./tools/fetch-libmem.sh` to get the dependencies.

```sh
git clone --recurse-submodules https://github.com/nathan818fr/ts3-server-hook.git
cd ts3-server-hook
./tools/fetch-libmem.sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel "$(nproc)"
```

### Try

An example script is provided to easily install and run a TeamSpeak 3 server
with the mod.

You can set the `TS3SERVERHOOK_DEBUG` environment variable to `true` to enable
debug logging. This will print all captured packets to the console.

```sh
./examples/ts3server.sh install
TS3SERVERHOOK_DEBUG=true ./examples/ts3server.sh run
```

### Dependencies

- [libmem] (using prebuilt binaries from 
  [nathan818fr/libmem-build][libmem-build])
- [asio]

[libmem]: https://github.com/rdbo/libmem
[libmem-build]: https://github.com/nathan818fr/libmem-build
[asio]: https://github.com/chriskohlhoff/asio

### Guidelines

Formatting is done with clang-format.
You can run `./tools/format.sh` to format.

## Background and purpose

Originally, I was working on another project to hide user badges on my TeamSpeak
server.
This led me to create a library that injects into the TeamSpeak server, hooks
the command packet handler and updates the commands before they are processed.
After that, I started exploring further to see if I could intercept and listen
to the audio. So I created this new project.

## License

Copyright (c) 2023 - Nathan Poirier &lt;nathan@poirier.io&gt;<br>
This project is licensed under the [AGPL-3.0](LICENSE.md), with
[exceptions](LICENSE_EXCEPTIONS.md).

## Disclaimer

This project is intended to be fun and informative.
Don't use it to abuse your users' privacy.

This software is provided as-is with no warranty or guarantees. I am not
responsible for any damage or illegal activities caused by the use of this
software. Use it at your own risk. It is your responsibility to obey all
applicable local, state, federal, and international laws in regard to the usage
of this software.

**This project is not affiliated with TeamSpeak in any way.**<br>
TeamSpeak is a registered trademark of TeamSpeak Systems GmbH.
