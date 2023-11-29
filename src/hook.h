#pragma once

#include <libmem/libmem.h>

#include <string>

#include "server.h"

namespace ts3_server_hook {

void init();

enum class Ts3Version {
    vUnknown,
    v3_13_6,
    v3_13_7,
};

inline std::string Ts3VersionName(Ts3Version version) {
    switch (version) {
        default:
            return "unknown";
        case Ts3Version::v3_13_6:
            return "3.13.6";
        case Ts3Version::v3_13_7:
            return "3.13.7";
    }
}

class Hook {
  public:
    void install();

    void start();

    void capture_packet(CaptureType type, lm_address_t packet, lm_address_t virtual_server, bool is_server_query);

  private:
    static Ts3Version detect_ts3_version(const lm_module_t *mod);

    Ts3Version ts3_version_ = Ts3Version::vUnknown;
    Server server_;
};

}  // namespace ts3_server_hook
