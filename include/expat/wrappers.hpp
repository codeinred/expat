#pragma once

#include <unistd.h>
#include <system_error>

#include <expat/wrapper_types.hpp>

namespace expat {
pipe_fd open_pipe() {
    int arr[2];
    if (pipe(arr) != -1) {
        return pipe_fd {arr[0], arr[1]};
    } else {
        throw std::system_error(
            errno,
            std::system_category(),
            "Unable to open pipe");
    }
}

pid_t fork_or_throw() {
    pid_t pid = fork();
    if (pid == -1) {
        throw std::system_error(
            errno,
            std::system_category(),
            "Unable to fork process");
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
}
