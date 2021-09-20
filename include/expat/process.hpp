#pragma once
#include <array>
#include <cstdio>
#include <string>
#include <system_error>
#include <unistd.h>

namespace expat {

struct pipe_fd {
    int read_end = 0;
    int write_end = 0;
};
pipe_fd open_pipe() {
    int arr[2];
    if (!pipe(arr)) {
        return pipe_fd {arr[0], arr[1]};
    } else {
        throw std::system_error(
            errno, std::system_category(), "Unable to open pipe");
    }
}

pid_t fork_or_throw() {
    pid_t pid = fork();
    if (pid == -1) {
        throw std::system_error(
            errno, std::system_category(), "Unable to fork process");
    }
    return pid;
}

int dup2_or_throw(int oldfd, int newfd) {
    int result = dup2(oldfd, newfd);
    if (result == -1) {
        throw std::system_error(errno, std::system_category(), "dup2 failed");
    }
    return result;
}
struct process_fd {
    int stdin = 0;
    int stdout = 0;
    int stderr = 0;
};

process_fd run_process(std::string_view pathname) {
    process_fd proc;

    pipe_fd stdin = open_pipe();
    pipe_fd stdout = open_pipe();
    pipe_fd stderr = open_pipe();

    if (fork_or_throw()) {
        // We know we're the parent because pid != 0

        // the parent gets the write end and the read ends
        return process_fd {
            stdin.write_end, // <br>
            stdout.read_end, // <br>
            stderr.read_end  // <br>
        };
    } else {
        // The pid returned from fork_or_throw() was 0, so we know we're the
        // child

        dup2_or_throw(stdin.read_end, 0);
        dup2_or_throw(stdout.write_end, 1);
        dup2_or_throw(stderr.write_end, 2);

        std::string pathname_arg = std::string(pathname);
        std::array<char*, 2> args = {pathname_arg.data(), nullptr};

        execv(pathname_arg.data(), args.data());

        // This only occurs when execv fails
        throw std::system_error(
            errno, std::system_category(), "Unable to run execv");
    }
}
} // namespace expat
