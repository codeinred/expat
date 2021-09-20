#pragma once

namespace expat {
struct pipe_fd {
    int read_end = 0;
    int write_end = 0;
};

struct standard_process_fds {
    int stdin = 0;
    int stdout = 0;
    int stderr = 0;
};

enum class fd_category : bool { fd_input, fd_output };

constexpr auto fd_input = fd_category::fd_input;
constexpr auto fd_output = fd_category::fd_output;


struct fd_desc {
    int fd {-1};
    fd_category category;
    constexpr operator int() const noexcept { return fd; }
};

constexpr fd_desc input_fd(int fd) { return fd_desc {fd, fd_input}; }
constexpr fd_desc output_fd(int fd) { return fd_desc {fd, fd_output}; }

constexpr fd_desc operator"" _input(unsigned long long fd) {
    return fd_desc {(int)fd, fd_input};
}
constexpr fd_desc operator"" _output(unsigned long long fd) {
    return fd_desc {(int)fd, fd_output};
}
} // namespace expat
