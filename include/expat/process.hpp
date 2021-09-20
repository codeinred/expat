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

process_fd run_process(std::string_view pathname) {
    process_fd proc;

    pipe_fd stdin = open_pipe();
    pipe_fd stdout = open_pipe();
    pipe_fd stderr = open_pipe();

    if (fork_or_throw()) {
        // We know we're the parent because pid != 0

        // We need to close the end we're not using
        close(stdin.read_end);
        close(stdout.write_end);
        close(stderr.write_end);
        // the parent gets the write end and the read ends
        return process_fd {
            stdin.write_end, // <br>
            stdout.read_end, // <br>
            stderr.read_end  // <br>
        };
    } else {
        // The pid returned from fork_or_throw() was 0, so we know we're the
        // child

        // We need to close the end we're not using
        close(stdin.write_end);
        close(stdout.read_end);
        close(stderr.read_end);

        dup2_or_throw(stdin.read_end, 0);
        dup2_or_throw(stdout.write_end, 1);
        dup2_or_throw(stderr.write_end, 2);

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

template <size_t N>
std::array<fd_desc, N> run_process(
    std::array<fd_desc, N> const& fds,
    std::string_view pathname) {
    process_fd proc;

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
} // namespace expat
