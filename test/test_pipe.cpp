#include <expat/process.hpp>
#include <fmt/core.h>

int main() {
    using namespace expat;

    std::string message = "Hello, world!";
    auto pipe = expat::open_pipe();

    if (expat::fork_or_throw()) {
        fmt::print("[Parent]\n");
        // We have to close the end we're not using before reading
        close(pipe.write_end);

        file_descriptor read {pipe.read_end};
        auto result = read.read_all();
        fmt::print("[Parent] Result: {}\n", result);
        if (result == message) {
            fmt::print("[Parent] Everything good!\n");
            return 0;
        } else {
            fmt::print("Expected '{}' but recieved '{}'\n", message, result);
            return 1;
        }
    } else {
        fmt::print("[Child] writing...\n");
        close(pipe.read_end);
        file_descriptor write {pipe.write_end};
        write.write_all(message);
        fmt::print("[Child] finished\n");
        return 0;
    }
}
