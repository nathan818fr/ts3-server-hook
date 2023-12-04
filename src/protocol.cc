#include "protocol.h"

#include <utility>

#include "util/cutils.h"

namespace ts3_server_hook {

buffer_ptr Protocol::write_hearbeat() {
    // Heartbeat packets are just empty packets (only frame header)
    return std::make_shared<PacketBuffer>(0);
}

buffer_ptr Protocol::write_handshake(const std::string& ts3_version) {
    auto ts3_version_len = saturating_cast<uint8_t>(ts3_version.length());
    auto packet = std::make_shared<PacketBuffer>(
        1 +              // packet_id
        1 +              // protocol_version
        1 +              // ts3_version len
        ts3_version_len  // ts3_version
    );
    packet->write_ubyte(0);                                     // packet_id (0 = HANDSHAKE)
    packet->write_ubyte(VERSION);                               // protocol version
    packet->write_ubyte(ts3_version_len);                       // ts3_version len
    packet->write_bytes(ts3_version.c_str(), ts3_version_len);  // ts3_version
    return packet;
}

buffer_ptr Protocol::write_capture(
    CaptureType type, int virtual_server_id, bool is_server_query, const void* data, int len
) {
    auto packet = std::make_shared<PacketBuffer>(
        1 +  // packet_id
        2 +  // virtual_server_id
        1 +  // flags
        len  // data
    );
    packet->write_ubyte(to_underlying(type));      // packet_id (same as CaptureType: 1 = COMMAND, 2 = VOICE)
    packet->write_ushort(virtual_server_id);       // virtual_server_id
    packet->write_ubyte(is_server_query ? 1 : 0);  // flags
    packet->write_bytes(data, len);                // data
    return packet;
}

}  // namespace ts3_server_hook
