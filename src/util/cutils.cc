#include "cutils.h"

#include <cstring>

namespace ts3_server_hook {

void unsetenv(char** envp, const char* prefix) {
    if (!envp || !envp[0] || !prefix) {
        return;
    }

    // Loop through the environment variables to find those matching the prefix and remove them.
    // To do this, the following elements (and the null terminator) are shifted to overwrite those that have been
    // deleted.
    size_t prefix_len = strlen(prefix);
    int i = 0, del = 0;
    for (; envp[i]; ++i) {
        if (!strncmp(envp[i], prefix, prefix_len)) {
            ++del;
        } else if (del) {
            envp[i - del] = envp[i];
        }
    }
    if (del) {
        envp[i - del] = nullptr;
    }
}

}  // namespace ts3_server_hook
