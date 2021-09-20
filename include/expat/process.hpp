#pragma once
#include <array>
#include <cstdio>
#include <span>
#include <string>
#include <system_error>
#include <unistd.h>

#include <expat/wrapper_types.hpp>
#include <expat/wrappers.hpp>

namespace expat {

template <size_t N>
std::array<fd_desc, N> run_process(
    std::array<fd_desc, N> const& fds,
    std::string_view pathname) {
    standard_process_fds proc;

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

        std::string pathname_arg = std::string(pathname);
        std::array<char*, 2> args = {pathname_arg.data(), nullptr};

        execv(pathname_arg.data(), args.data());

        // This only occurs when execv fails
        throw std::system_error(
            errno,
            std::system_category(),
            "Unable to run execv");
    }
}

standard_process_fds run_process(std::string_view pathname) {
    auto [stdin, stdout, stderr] =
        run_process(std::array {0_input, 1_output, 2_output}, pathname);
    return standard_process_fds {stdin, stdout, stderr};
}
} // namespace expat
