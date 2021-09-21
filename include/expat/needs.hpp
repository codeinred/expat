#pragma once
#include <conduit/util/stdlib_coroutine.hpp>

#include <conduit/coroutine.hpp>
#include <conduit/future.hpp>

#include <expat/process.hpp>

#include <string>

#include <boost/asio/posix/stream_descriptor.hpp>

namespace expat {
namespace co = conduit;
namespace asio = boost::asio;
namespace posix = boost::asio::posix;

using boost::system::error_code;
using fd_stream = posix::stream_descriptor;


template <class Stream, class Buffer>
class async_read_some {
    Stream& stream;
    Buffer& buffer;
    size_t result {};

   public:
    async_read_some(Stream& stream, Buffer& buffer)
      : stream(stream)
      , buffer(buffer) {}

    constexpr bool await_ready() const noexcept { return false; }
    constexpr void await_suspend(std::coroutine_handle<> handle) {
        stream.async_read_some(
            asio::buffer(buffer),
            [this, handle](error_code const& err, size_t bytes_read) {
                if (err) {
                    result = 0;
                } else {
                    result = bytes_read;
                }
                handle.resume();
            });
    }
    constexpr auto await_resume() const noexcept {
        return std::string_view(buffer.data(), result);
    }
};

auto read_all_output(asio::io_context& context, char** argv)
    -> co::future<std::string> {
    auto [child_out, child_err] = run_process(
        std::array {1_output, 2_output},
        argv);

    auto output = fd_stream(context, child_out.fd);

    std::array<char, 1024> buffer;
    std::string result;
    while (true) {
        std::string_view text = co_await async_read_some(output, buffer);

        // If No bytes are read, we break so we can return the result
        if (text.size() == 0)
            break;

        result += text;
    }

    co_return result;
}
} // namespace expat
