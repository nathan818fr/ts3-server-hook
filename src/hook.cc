#include "hook.h"

#include <iostream>

#include "constant.h"
#include "debug.h"
#include "server.h"
#include "util/lmutils.h"

namespace ts3_server_hook {

void init() {  // it's our main() function
    log() << "Version: " << TS3SH_VERSION << " https://github.com/nathan818fr/ts3-server-hook" << log_endl;
    log() << "Successfully injected" << log_endl;
    (new Hook)->install();
}

static Hook *g_hook = nullptr;

typedef int (*start_server_fn)(void *a, uint32_t b, void *c);
static start_server_fn g_real_start_server;

static int hooked_start_server(void *a, uint32_t b, void *c) {
    g_hook->start();
    return g_real_start_server(a, b, c);
}

typedef void (*handle_command_fn)(void *a, void *b, void *c, void *d, uint8_t e);
static handle_command_fn g_real_handle_command;

static void hooked_handle_command(void *a, void *b, void *c, void *d, uint8_t e) {
    auto packet = reinterpret_cast<lm_address_t>(b);
    auto virtual_server = reinterpret_cast<lm_address_t>(c);
    g_hook->capture_packet(CaptureType::COMMAND, packet, virtual_server, e == 1);
    return g_real_handle_command(a, b, c, d, e);
}

typedef void (*handle_voice_fn)(void *a, void *b, void *c, void *d);
static handle_voice_fn g_real_handle_voice;

static void hooked_handle_voice(void *a, void *b, void *c, void *d) {
    auto packet = reinterpret_cast<lm_address_t>(b);
    auto virtual_server = reinterpret_cast<lm_address_t>(c);
    g_hook->capture_packet(CaptureType::VOICE, packet, virtual_server, false);
    return g_real_handle_voice(a, b, c, d);
}

void Hook::install() {
    g_hook = this;

    lm_process_t proc;
    if (!LM_GetProcess(&proc)) {
        log() << "LM_GetProcess failed" << log_endl;
        return;
    }

    lm_module_t mod;
    if (!LM_FindModule(proc.name, &mod)) {
        log() << "LM_FindModule failed" << log_endl;
        return;
    }

    ts3_version_ = detect_ts3_version(&mod);
    log() << mod.name << " - Detected server version: " << Ts3VersionName(ts3_version_) << log_endl;

    switch (ts3_version_) {
        default: {
            log() << "------------------------------------------------------------" << log_endl;
            log() << "/!\\ This server version is not supported." << log_endl;
            log() << "Refer to the project's GitHub page for more information:" << log_endl;
            log() << "https://github.com/nathan818fr/ts3-server-hook" << log_endl;
            log() << "------------------------------------------------------------" << log_endl << log_endl;
            break;
        }
        case Ts3Version::v3_13_6:
        case Ts3Version::v3_13_7: {
            lmu_hook_function(
                0x004b3b90, reinterpret_cast<lm_address_t>(hooked_start_server),
                reinterpret_cast<lm_address_t *>(&g_real_start_server)
            );
            lmu_hook_function(
                0x0069b460, reinterpret_cast<lm_address_t>(hooked_handle_command),
                reinterpret_cast<lm_address_t *>(&g_real_handle_command)
            );
            lmu_hook_function(
                0x0069de60, reinterpret_cast<lm_address_t>(hooked_handle_voice),
                reinterpret_cast<lm_address_t *>(&g_real_handle_voice)
            );
            break;
        }
    }
}

void Hook::start() {
    server_.start(Ts3VersionName(ts3_version_));
}

void Hook::capture_packet(CaptureType type, lm_address_t packet, lm_address_t virtual_server, bool is_server_query) {
    // packet_len is a trusted value, so we can use it without risk of reading out of bounds
    auto *packet_data = reinterpret_cast<void *>(*reinterpret_cast<lm_address_t *>(packet + 0x8));
    int packet_len = *reinterpret_cast<int *>(packet + 0x10);
    int virtual_server_id = *reinterpret_cast<uint16_t *>(virtual_server + 0x58);
    server_.capture(type, virtual_server_id, is_server_query, packet_data, packet_len);
}

Ts3Version Hook::detect_ts3_version(const lm_module_t *mod) {
    constexpr lm_uint8_t patternA[]{0x55, 0x41, 0x57, 0x41, 0x56, 0x41, 0x55, 0x41,
                                    0x54, 0x53, 0x48, 0x81, 0xec, 0xc8, 0x01, 0x00};
    if (lmu_memory_equals(mod, 0x00a13bf0, patternA, sizeof(patternA))) {
        return Ts3Version::v3_13_6;
    }
    if (lmu_memory_equals(mod, 0x00a13c10, patternA, sizeof(patternA))) {
        return Ts3Version::v3_13_7;
    }
    return Ts3Version::vUnknown;
}

}  // namespace ts3_server_hook
