#include <filesystem>
#include <fmt/core.h>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;

void list_directory(fs::path const& p) {
    for (auto const& entry : fs::directory_iterator(p)) {
        fmt::print("{}\n", entry.path().string());
    }
}
void list(std::vector<std::string_view> const& paths) {
    fs::path path;
    for (auto sv : paths) {
        path = sv;
        if (!fs::exists(path)) {
            fmt::print("Path not found: '{}'\n", path.string());
        } else if (fs::is_directory(path)) {
            list_directory(path);
        } else {
            fmt::print("{}\n", path.string());
        }
    }
}
int main(int argc, char** argv) {
    std::vector<std::string_view> paths;
    if (argc == 1) {
        paths.push_back(".");
    } else {
        for (int i = 1; i < argc; i++) {
            paths.push_back(argv[i]);
        }
    }
    list(paths);
}
