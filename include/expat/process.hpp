#pragma once
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
} // namespace expat
