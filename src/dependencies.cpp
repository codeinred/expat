#include <expat/needs.hpp>

auto read_and_print_output(boost::asio::io_context& context, char** argv) {
    return [&context, argv]() -> boost::asio::awaitable<void> {
        fmt::print("[Reading output for {}]\n", argv[0]);
        std::string output = co_await expat::read_all_output(context, argv);

        fmt::print("Output:\n\n{}", output);

        context.stop();
    };
}
int main(int argc, char** argv) {
    using namespace expat;

    asio::io_context context;

    asio::co_spawn(
        context,
        read_and_print_output(context, argv + 1),
        asio::detached);

    context.run();
}
