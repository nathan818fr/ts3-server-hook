#pragma once

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>

namespace ts3_server_hook {

extern bool DEBUG_ENABLED;

bool getEnvFlag(const char* name);

inline std::ostream& log() {
    return std::clog << "[ts3-server-hook] ";
}

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>& log_endl(std::basic_ostream<CharT, Traits>& os) {
    return flush(os.put(os.widen('\n')));
}

struct LogBool_ {
    bool value_;
};

inline LogBool_ log_bool(bool b) {
    return {b};
}

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, LogBool_ v) {
    return os << (v.value_ ? "true" : "false");
}

struct LogHex_ {
    uint64_t value_;
};

inline LogHex_ log_hex(uint64_t n) {
    return {n};
}

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, LogHex_ v) {
    std::ios old_state(nullptr);
    old_state.copyfmt(os);
    os << "0x" << std::hex << v.value_;
    os.copyfmt(old_state);
    return os;
}

struct LogStrerror_ {
    int errno_;
};

inline LogStrerror_ log_strerror(int errno_) {
    return {errno_};
}

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, LogStrerror_ v) {
    return os << "errno=" << v.errno_ << " \"" << std::strerror(v.errno_) << '"';
}

struct LogHexDump_ {
    const void* data_;
    size_t size_;
    bool absolute_offset_;
};

inline LogHexDump_ log_hex_dump(const void* data, size_t size, bool absolute_offset = false) {
    return {data, size, absolute_offset};
}

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, LogHexDump_ v) {
    auto* data = static_cast<const uint8_t*>(v.data_);
    auto size = v.size_;
    auto absolute_offset = v.absolute_offset_;

    constexpr size_t bytes_per_line = 16;
    constexpr size_t bytes_group_size = 2;

    // Calculate the width of the offset field
    int prefix_w;
    if (absolute_offset) {
        prefix_w = sizeof(void*) * 2;
    } else {
        prefix_w = std::max(size > 0 ? static_cast<int>(std::log10(size)) + 1 : 1, 4);
    }

    // Declare the ASCII Field buffer
    char ascii[bytes_per_line + 1] = {};

    // Save the state of the output stream and set the default formatting
    std::ios old_state(nullptr);
    old_state.copyfmt(os);
    os << std::setfill('0') << std::hex;

    // Loop through the data, one byte at a time; i is incremented in the loop
    for (size_t i = 0; i < size;) {
        // 1. Offset Field
        if (i % bytes_per_line == 0) {
            if (i != 0) {
                os << '\n';
            }

            // At the start of each line, print the offset
            os << std::setw(prefix_w);
            if (absolute_offset) {
                os << reinterpret_cast<uintptr_t>(data + i);
            } else {
                os << i;
            }
            os << ':';
        }

        // 2. Hex Field
        if (i % bytes_group_size == 0) {
            // Before each group of bytes, print a space
            // This will also print the space to separate from the offset field
            os << ' ';
        }
        os << std::setw(2) << static_cast<unsigned int>(data[i]);  // Print the byte as a two-digit hex

        // 3. ASCII Field
        if (data[i] >= ' ' && data[i] <= '~') {
            // If the byte is printable, add it to the ASCII buffer
            ascii[i % bytes_per_line] = static_cast<char>(data[i]);
        } else {
            // Otherwise, add a dot to the ASCII buffer
            ascii[i % bytes_per_line] = '.';
        }

        ++i;
        if (i >= size) {
            // Fill the rest of the Hex Field with spaces
            for (; i % bytes_per_line != 0; ++i) {
                if (i % bytes_group_size == 0) {
                    os << ' ';
                }
                os << ' ' << ' ';
            }
        }
        if (i % bytes_per_line == 0) {
            // At the end of each line, print the ASCII buffer
            os << " | " << ascii;
        }
    }

    os.copyfmt(old_state);  // Restore the state of the output stream
    return os;
}

}  // namespace ts3_server_hook
