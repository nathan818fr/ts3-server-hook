#include <dlfcn.h>

#include <cstdio>
#include <cstring>

#include "hook.h"
#include "util/cutils.h"

// ts3-server-hook must be start just before the invocation of the ts3server main function, this ensures that
// everything is initialized.
// The library uses LD_PRELOAD to override __libc_start_main and replace the main function with a custom one.
// The custom main function checks if the executable is ts3server and if so, it unsets LD_PRELOAD and calls our
// init() function.

namespace ts3_server_hook {

typedef int (*main_fn_t)(int, char**, char**);
static main_fn_t g_real_main = nullptr;

static int hooked_main(int argc, char** argv, char** envp) {
    char* basename = strrchr(argv[0], '/');
    if (basename) {
        ++basename;
    } else {
        basename = argv[0];
    }
    if (!strcmp(basename, "ts3server") || !strcmp(basename, "ts3server_linux_amd64")) {
        unsetenv(envp, "LD_PRELOAD=");
        init();
    }
    return g_real_main(argc, argv, envp);
}

}  // namespace ts3_server_hook

extern "C" int __libc_start_main(  // NOLINT(*-reserved-identifier)
    ts3_server_hook::main_fn_t main_fn,
    int argc,
    char** argv,
    void (*init)(),
    void (*fini)(),
    void (*rtld_fini)(),
    void* stack_end
) {
    auto* real_libc_start_main = reinterpret_cast<decltype(__libc_start_main)*>(dlsym(RTLD_NEXT, "__libc_start_main"));
    if (!ts3_server_hook::g_real_main) {
        ts3_server_hook::g_real_main = main_fn;
        main_fn = &ts3_server_hook::hooked_main;
    }
    return real_libc_start_main(main_fn, argc, argv, init, fini, rtld_fini, stack_end);
}
