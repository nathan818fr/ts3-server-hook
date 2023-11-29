#include "debug.h"

#include <cstdlib>

namespace ts3_server_hook {

bool DEBUG_ENABLED = getEnvFlag("TS3SERVERHOOK_DEBUG");

bool getEnvFlag(const char *name) {
    char *value = std::getenv(name);
    return value && !strcmp(value, "true");
}

}  // namespace ts3_server_hook
