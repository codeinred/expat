#pragma once
#include <expat/system_error.hpp>
#include <unistd.h>
namespace expat {

struct pipe_fd {
    int read_end;
    int write_end;
};
pipe_fd open_pipe() {
    int arr[2];
    if(!pipe(arr)) {
        return pipe_fd{arr[0], arr[1]};
    } else {
        throw_errno("Unable to open pipe");
    }
}


struct process {

};
}
