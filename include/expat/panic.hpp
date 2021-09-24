#pragma once

#include <array>
#include <cstdio>
#include <string_view>
#include <system_error>

#include <execinfo.h>
#include <unistd.h>

namespace expat {
inline void print_backtrace(int err) {
    std::array<void*, 64> array;
    int size = backtrace(array.data(), array.size());

    backtrace_symbols_fd(array.data(), size, STDERR_FILENO);
}

[[noreturn]] inline void panic(std::string_view sv, int err = errno) {
#ifndef NDEBUG
    print_backtrace(err);
#endif

    throw std::system_error(err, std::system_category(), sv.data());
}
[[noreturn]] inline void panic(std::string const& s, int err = errno) {
#ifndef NDEBUG
    print_backtrace(err);
#endif

    throw std::system_error(err, std::system_category(), s);
}

} // namespace uio
