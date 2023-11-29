#pragma once

#include <algorithm>
#include <cstdint>

namespace ts3_server_hook {

/**
 * A really simple buffer class for writing socket server packets.
 */
class PacketBuffer {
  public:
    explicit PacketBuffer(size_t packet_size) : len_(2 + packet_size), data_(new uint8_t[len_]{}) {
        write_ushort(packet_size);
    }

    PacketBuffer(const PacketBuffer& other) : pos_(other.pos_), len_(other.len_), data_(new uint8_t[other.len_]) {
        std::copy(other.data_, other.data_ + other.len_, data_);
    }

    ~PacketBuffer() {
        delete[] data_;
    }

    [[nodiscard]] size_t len() const {
        return len_;
    }

    [[nodiscard]] const uint8_t* data() const {
        return data_;
    }

    void write_ubyte(uint8_t i);

    void write_ushort(uint16_t i);

    void write_bytes(const void* src, size_t count);

  private:
    size_t pos_ = 0;
    size_t len_;
    uint8_t* data_;
};

}  // namespace ts3_server_hook
