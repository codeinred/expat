#pragma once
#include <expat/literals.hpp>
#include <expat/panic.hpp>
#include <expat/process.hpp>

#include <array>
#include <cstdio>
#include <string>

#include <fmt/format.h>

namespace expat {
std::string read_all_output(char** argv) {
    auto [child_out, child_err] = run_process(
        std::array {1_output, 2_output},
        argv);

    std::array<char, 16_kb> buffer;
    std::string result;
    while (ssize_t count = read(child_out, buffer.data(), buffer.size())) {
        if (count < 0)
            panic(
                fmt::format("Unable to read from child process '{}'", argv[0]));

        result += std::string_view(buffer.data(), count);
    }
    close(child_out);
    close(child_err);
    return result;
}
} // namespace expat
