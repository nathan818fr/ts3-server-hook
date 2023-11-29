#pragma once

#include <memory>
#include <string>

#include "util/packet-buffer.h"

namespace ts3_server_hook {

typedef std::shared_ptr<const PacketBuffer> buffer_ptr;

enum class CaptureType {
    COMMAND = 1,
    VOICE = 2,
};

class Protocol {
  public:
    static constexpr uint8_t VERSION = 2;
    static constexpr int64_t HEARTBEAT_INTERVAL_MS = 5000;

    static buffer_ptr write_hearbeat();

    static buffer_ptr write_handshake(const std::string& ts3_version);

    static buffer_ptr write_capture(
        CaptureType type, int virtual_server_id, bool is_server_query, const void* data, int len
    );
};

}  // namespace ts3_server_hook
