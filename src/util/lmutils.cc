#include "lmutils.h"

#include <memory>

#include "debug.h"

namespace ts3_server_hook {

bool lmu_memory_equals(const lm_module_t *mod, lm_address_t addr, const lm_byte_t *pattern, lm_size_t pattern_len) {
    if (addr < mod->base) {
        return false;
    }
    if (addr + pattern_len >= mod->end) {
        return false;
    }

    const auto bytes = std::make_unique<lm_byte_t[]>(pattern_len);
    pattern_len = LM_ReadMemory(addr, bytes.get(), pattern_len);
    return pattern_len > 0 && !memcmp(bytes.get(), pattern, pattern_len);
}

bool lmu_hook_function(lm_address_t from, lm_address_t to, lm_address_t *ptrampoline) {
    lm_size_t res = LM_HookCode(from, to, ptrampoline);
    if (res == 0) {
        log() << "LM_HookCode failed" << log_endl;
        return false;
    }
    if (ptrampoline && !*ptrampoline) {
        log() << "LM_HookCode failed (null trampoline)" << log_endl;
        return false;
    }
    return true;
}

}  // namespace ts3_server_hook
