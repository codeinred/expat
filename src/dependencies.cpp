#include <fmt/color.h>
#include <fmt/core.h>

#include <expat/needs.hpp>

auto co_main(boost::asio::io_context& context, int argc, char** argv)
    -> conduit::coroutine {

    // The program name is stored in argv[1], and the arguments are stored in
    // argv[2] ..., so we pass argv + 1
    auto future = expat::read_all_output(context, argv + 1);
    std::string output = co_await future;

    fmt::print("Output:\n\n{}", output);

    context.stop();
}
int main(int argc, char** argv) {
    using namespace expat;

    asio::io_context context;

    co_main(context, argc, argv);

    context.run();
}
