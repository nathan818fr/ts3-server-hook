#pragma once

#include <libmem/libmem.hpp>

namespace ts3_server_hook {

bool lmu_memory_equals(const lm_module_t *mod, lm_address_t addr, const lm_byte_t *pattern, lm_size_t pattern_len);

bool lmu_hook_function(lm_address_t from, lm_address_t to, lm_address_t *ptrampoline);

}  // namespace ts3_server_hook
