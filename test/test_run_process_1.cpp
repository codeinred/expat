#include <expat/process.hpp>
#include <fmt/core.h>

int main(int argc, char** argv) {
    using namespace expat;
    auto fds = expat::run_process(argv[1]);
    std::string sout;
    std::string serr;

    bool out_open = true, err_open = true, user_open = true;
    std::array<char, 1024> buffer;
    while (out_open && err_open) {
        if (user_open) {
            auto sv = read_or_throw(0, buffer);
            if (sv.size() == 0) {
                user_open = false;
                close(fds.stdin);
            } else {
                file_descriptor {fds.stdin}.write_all(sv);
            }
        }
        if (user_open) {
            auto sv = read_or_throw(0, buffer);
            if (sv.size() == 0) {
                user_open = false;
                close(fds.stdin);
            } else {
                file_descriptor {fds.stdin}.write_all(sv);
            }
        }
        if (out_open) {
            auto sv = read_or_throw(fds.stdout, buffer);
            out_open = sv.size() > 0;
            sout += sv;
        }
        if (err_open) {
            auto sv = read_or_throw(fds.stderr, buffer);
            err_open = sv.size() > 0;
            serr += sv;
        }
    }
    fmt::print("[stdout]\n{}\n", sout);
    fmt::print("[stderr]\n{}\n", serr);
}
