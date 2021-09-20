#pragma once
#include <fmt/core.h>

#include <array>
#include <cstdio>
#include <filesystem>
#include <span>
#include <string>
#include <system_error>
#include <unistd.h>

#include <expat/wrapper_types.hpp>
#include <expat/wrappers.hpp>

namespace expat {
namespace fs = std::filesystem;
fs::path get_pathname(fs::path name) {
    if (name.is_absolute() || fs::exists(name)) {
        return name;
    }
    std::string_view path = getenv("PATH");
    if (path.data() == nullptr) {
        throw std::runtime_error("PATH environment variable not defined");
    }

    size_t p0 = 0;
    while (p0 < path.size()) {
        // Get the next entry
        size_t p1 = path.find(':', p0);
        // Skip empty entries
        if (p1 == p0)
            continue;
        if (p1 == path.npos)
            p1 = path.size();
        fs::path entry = path.substr(p0, p1 - p0) / name;
        p0 = p1 + 1;

        if (fs::exists(entry)) {
            return entry;
        }
    }

    throw std::runtime_error(fmt::format(
        "Unable to find '{}' in $PATH with PATH='{}'",
        name.c_str(),
        path));
}

template <size_t N>
std::array<fd_desc, N> run_process(
    std::array<fd_desc, N> const& fds,
    char** argv) {
    standard_process_fds proc;

    auto program = get_pathname(argv[0]);

    std::array<pipe_fd, N> pipes;
    for (auto& pipe : pipes) {
        pipe = open_pipe();
    }

    if (fork_or_throw()) {
        // We know we're the parent because pid != 0

        // We need to close the end we're not using
        std::array<fd_desc, N> result;
        for (int i = 0; i < N; i++) {
            if (fds[i].category == fd_input) {
                close(pipes[i].read_end);
                result[i] = output_fd(pipes[i].write_end);
            } else {
                close(pipes[i].write_end);
                result[i] = input_fd(pipes[i].read_end);
            }
        }
        // the parent gets the write end and the read ends
        return result;
    } else {
        // The pid returned from fork_or_throw() was 0, so we know we're the
        // child

        // We need to close the end we're not using
        for (int i = 0; i < N; i++) {
            if (fds[i].category == fd_input) {
                close(pipes[i].write_end);
                dup2_or_throw(pipes[i].read_end, fds[i]);
            } else {
                close(pipes[i].read_end);
                dup2_or_throw(pipes[i].write_end, fds[i]);
            }
        }

        execv(program.c_str(), argv);

        // This only occurs when execv fails
        throw std::system_error(
            errno,
            std::system_category(),
            "Unable to run execv");
    }
}

standard_process_fds run_process(char** argv) {
    auto [stdin, stdout, stderr] =
        run_process(std::array {0_input, 1_output, 2_output}, argv);
    return standard_process_fds {stdin, stdout, stderr};
}
} // namespace expat
