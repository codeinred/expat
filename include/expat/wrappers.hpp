#pragma once

#include <system_error>
#include <unistd.h>
#include <array>
#include <span>

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

std::string_view read_or_throw(int fd, std::span<char> buffer) {
    ssize_t bytes_read = ::read(fd, buffer.data(), buffer.size());
    if (bytes_read == -1) {
        throw std::system_error(
            errno,
            std::system_category(),
            "Error when reading from fd");
    }
    return std::string_view(buffer.data(), bytes_read);
}
std::string_view write_or_throw(int fd, std::string_view data) {
    ssize_t result = ::write(fd, data.data(), data.size());

    if (result == -1) {
        throw std::system_error(
            errno,
            std::system_category(),
            "Error when writing to fd");
    }

    data.remove_prefix(result);
    return data;
}
void write_all(int fd, std::string_view data) {
    while (data.size() > 0) {
        data = write_or_throw(fd, data);
    }
}
std::string read_all(int fd) {
    std::array<char, 4096> buffer;
    std::string result;
    while (true) {
        auto sv = read_or_throw(fd, buffer);
        if (sv.size() == 0)
            break;
        result += sv;
    }
    return result;
}
}
