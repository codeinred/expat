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
} // namespace expat
