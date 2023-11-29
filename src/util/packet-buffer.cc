#include "packet-buffer.h"

#include <arpa/inet.h>

#include <string>

namespace ts3_server_hook {

static constexpr void fail_oob([[maybe_unused]] const std::string& fn) {
#ifdef TS3SH_DEV
    throw std::runtime_error("PacketBuffer: " + fn + ": out of bounds");
#endif
}

void PacketBuffer::write_ubyte(uint8_t i) {
    if (pos_ >= len_) {
        return fail_oob("write_int8");
    }
    data_[pos_] = i;
    ++pos_;
}

void PacketBuffer::write_ushort(uint16_t i) {
    if (pos_ + 2 > len_) {
        return fail_oob("write_int16");
    }
    *reinterpret_cast<uint16_t*>(data_ + pos_) = htons(i);
    pos_ += 2;
}

void PacketBuffer::write_bytes(const void* src, size_t count) {
    if (pos_ + count > len_) {
        return fail_oob("write_bytes");
    }
    std::copy(static_cast<const uint8_t*>(src), static_cast<const uint8_t*>(src) + count, data_ + pos_);
    pos_ += count;
}

}  // namespace ts3_server_hook
