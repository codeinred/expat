#pragma once

namespace expat {
struct pipe_fd {
    int read_end = 0;
    int write_end = 0;
};


struct process_fd {
    int stdin = 0;
    int stdout = 0;
    int stderr = 0;
};

enum class fd_category : bool { fd_input, fd_output };

constexpr auto fd_input = fd_category::fd_input;
constexpr auto fd_output = fd_category::fd_output;
} // namespace expat
