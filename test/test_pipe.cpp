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

        auto result = read_all(pipe.read_end);
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
        write_all(pipe.write_end, message);
        fmt::print("[Child] finished\n");
        return 0;
    }
}
