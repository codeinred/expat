#pragma once

namespace expat {
struct pipe_fd {
    int read_end = 0;
    int write_end = 0;
};
} // namespace expat
