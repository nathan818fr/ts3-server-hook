#pragma once

#include <limits>
#include <type_traits>

namespace ts3_server_hook {

void unsetenv(char** envp, const char* prefix);

template <class D, class S>
D saturating_cast(S value) {
    constexpr S min = std::numeric_limits<D>::min();
    if (value <= min) {
        return min;
    }
    constexpr S max = std::numeric_limits<D>::max();
    if (value >= max) {
        return max;
    }
    return static_cast<D>(value);
}

template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

#define TS3SH_HANDLER(fn) [this]<typename... TArgs>(TArgs&&... args) { return this->fn(std::forward<TArgs>(args)...); }

}  // namespace ts3_server_hook
