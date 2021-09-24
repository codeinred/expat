#include <fmt/color.h>
#include <fmt/core.h>

#include <expat/needs.hpp>

int main(int argc, char** argv) {

    std::string output = expat::read_all_output(argv + 1);

    fmt::print("Output:\n\n{}", output);

}
