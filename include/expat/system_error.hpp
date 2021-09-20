#include <system_error>
#include <unistd.h>

namespace expat {
void throw_errno() {
    throw std::system_error(errno, std::generic_category());
}
void throw_errno(const char* what) {
    throw std::system_error(errno, std::generic_category(), what);
}
void throw_errno(std::string const& what) {
    throw std::system_error(errno, std::generic_category(), what);
}
} // namespace expat
