#include <conduit/util/stdlib_coroutine.hpp>
#include <expat/process.hpp>
#include <fmt/core.h>

#define BOOST_ASIO_HAS_CO_AWAIT
#include <boost/asio/io_context.hpp>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/coro.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

namespace asio = boost::asio;
namespace posix = boost::asio::posix;

using posix_stream = posix::stream_descriptor;


template <class Context>
struct stop_on_destruction {
    Context& context;
    ~stop_on_destruction() { context.stop(); }
};
template <class Context>
stop_on_destruction(Context&) -> stop_on_destruction<Context>;

auto make_reader(
    asio::io_context& context, posix_stream& stream, std::string prefix) {
    return [&context, &stream, prefix]() -> asio::awaitable<void> {
        stop_on_destruction stop_token {context};
        std::string buffer(1024, '\0');

        while (stream.is_open()) {
            std::string_view result = std::string_view(
                buffer.data(),
                co_await stream.async_read_some(
                    asio::buffer(buffer), asio::use_awaitable));

            size_t pos = 0;
            do {
                size_t new_pos = result.find('\n', pos);
                auto line = result.substr(pos, new_pos - pos);
                fmt::print("{}{}\n", prefix, line);
                pos = new_pos + 1;
            } while (pos < result.size());
        }
    };
}
int main(int argc, char** argv) {
    using namespace expat;
    auto fds = expat::run_process(argv[1]);

    asio::io_context context;
    posix_stream stdin = posix_stream(context, ::dup(STDIN_FILENO));
    posix_stream child_in = posix_stream(context, fds.stdin);
    posix_stream child_out = posix_stream(context, fds.stdout);
    posix_stream child_err = posix_stream(context, fds.stderr);

    auto write_message = [&]() -> asio::awaitable<void> {
        stop_on_destruction stop_token {context};
        std::string buffer(1024, '\0');
        while (stdin.is_open()) {
            size_t count = co_await stdin.async_read_some(
                asio::buffer(buffer), asio::use_awaitable);
            if (count == 0) {
                fmt::print("reached EOF\n");
                child_in.close();
                break;
            }

            std::string_view message = std::string_view(buffer.data(), count);
            while (child_in.is_open() && message.size() > 0) {
                auto bytes_written = co_await child_in.async_write_some(
                    asio::buffer(message), asio::use_awaitable);
                message.remove_prefix(bytes_written);
            }
        }
    };

    auto read_output = make_reader(context, child_out, "[stdout] ");
    auto read_err = make_reader(context, child_err, "[stderr] ");
    asio::co_spawn(context, read_output, asio::detached);
    asio::co_spawn(context, read_err, asio::detached);
    asio::co_spawn(context, write_message, asio::detached);

    context.run();
}
