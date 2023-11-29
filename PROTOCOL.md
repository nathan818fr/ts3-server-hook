# TS3 Server Hook - Protocol

A binary protocol used by the hook to send data to the client.

All packets are sent from the hook to the client (so the client should not send anything to the server).

## Data types

| Name   | Size (bytes) | Description                                             |
|--------|--------------|---------------------------------------------------------|
| ubyte  | 1            | Unsigned 8-bit integer                                  |
| ushort | 2            | Unsigned 16-bit integer                                 |
| string | _variable_   | UTF-8 string prefixed with its size in bytes as a ubyte |

## Packet format

| Field       | Type       | Description                                       |
|-------------|------------|---------------------------------------------------|
| packet_size | ushort     | Size of the `packet_id` + `data` fields in bytes  |
| packet_id   | ubyte      | Packet ID _(if packet_size >= 1)_                 |
| packet_data | _variable_ | Depends on the packet ID  _(if packet_size >= 2)_ |

## Packets

### Handshake (0x00)

| Field            | Type   | Description                              |
|------------------|--------|------------------------------------------|
| protocol_version | ubyte  | The protocol version (currently `2`)     |
| ts3_version      | string | The TS3 server version (e.g. `"3.13.7"`) |

### Command Capture (0x01)

| Field             | Type       | Description                                            |
|-------------------|------------|--------------------------------------------------------|
| virtual_server_id | ushort     | The TS3 virtual server ID                              |
| flags             | ubyte      | Capture flags (see below)                              |
| data              | _variable_ | Capture data (see [TS3 Server Spec](#ts3-server-spec)) |

**Capture flags** is a bitfield with the following flags:
- Bit 1 (0x01): `is_server_query`, set if the capture is from a server query

### Voice Capture (0x02)

Same as [Command Capture](#command-capture-0x01).

### Heartbeat (empty)

Missing `packet_id` _(when packet_size == 0)_ means a heartbeat packet.

Heartbeat packets are sent to the client every 5 seconds.
It is recommended for the clients to disconnect if no heartbeat is received for
15 seconds.

## TS3 Server Spec

From [ReSpeak tsdeclarations][ts3protocol.md].

[ts3protocol.md]: https://github.com/ReSpeak/tsdeclarations/blob/e19149d13ec114fd9756bc726e8f86bf47ae9181/ts3protocol.md

### Command

A TS3 query-like command, encoded in UTF-8.

### Voice

| Field            | Type       | Description                           |
|------------------|------------|---------------------------------------|
| voice_packet_id  | ushort     | The voice packet ID                   |
| voice_codec      | ubyte      | The voice codec type                  |
| voice_data       | _variable_ | The voice data (depends on the codec) |

`voice_codec` 4 is Opus (1 channel) and 5 is Opus Music (2 channels).
In both cases, the `voice_data` is an Opus frame (48kHz, 20ms).
