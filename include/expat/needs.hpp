#pragma once
#include <conduit/util/stdlib_coroutine.hpp>

#include <conduit/coroutine.hpp>

#include <expat/process.hpp>

#include <string>

#define BOOST_ASIO_HAS_CO_AWAIT
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/coro.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

namespace expat {
namespace asio = boost::asio;
namespace posix = boost::asio::posix;

using fd_stream = posix::stream_descriptor;

auto read_all_output(asio::io_context& context, char** argv)
    -> asio::awaitable<std::string> {
    auto [child_out, child_err] = run_process(
        std::array {1_output, 2_output},
        argv);

    auto output = fd_stream(context, child_out.fd);

    std::array<char, 1024> buffer;
    std::string result;
    while (output.is_open()) {
        size_t num_bytes = co_await output.async_read_some(
            asio::buffer(buffer),
            asio::use_awaitable);


        // If No bytes are read, we break so we can return the result
        if (num_bytes == 0)
            break;

        auto section_read = std::string_view(buffer.data(), num_bytes);
        fmt::print("Read {}\n", section_read);
        fmt::print("Output open? {}\n", output.is_open());
        result += section_read;
    }

    co_return result;
}
} // namespace expat
