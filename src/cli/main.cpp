#include "Args.h"
#include <File.h>
#include <Filesystem.h>
#include <Directory.h>
#include <fmt/format.h>
#include <fmt/std.h>

using namespace flippy;

template<typename... Args>
void print_indented(int indent, fmt::format_string<Args...> format_str, Args&&... args)
{
    fmt::print("{:{}}", "", indent);
    fmt::print(format_str, std::forward<Args>(args)...);
}

int main(int argc, char** argv, char** envp)
{
    const auto args = ArgsParser::parse(
        argc, argv, envp, "FLIPPY_",
        [](const char* msg, std::size_t off, char* arg) {
            fmt::print(stderr, "Flippy: invalid argument, {} at {} in '{}'\n", msg, off, arg);
        });
    if (args.freeformSize() < 1) {
        fmt::print(stderr, "Flippy: missing image name\n");
        return 1;
    }
    fmt::print("Flippy: opening image '{}'\n", args.freeformValue(0));
    const auto& imageName = args.freeformValue(0);
    auto maybedir = Filesystem::root(imageName);
    if (!maybedir.has_value()) {
        fmt::print(stderr, "Flippy: cannot open image '{}': {}\n", imageName, maybedir.error().message());
        return 1;
    }
    auto rootdir = std::move(maybedir).value();

    std::function<int(const std::shared_ptr<Directory>&)> printDir;
    int iter = 0, indent = 0;
    printDir = [&printDir, &iter, &indent](const std::shared_ptr<Directory>& dir) -> int {
        if (iter++ == 1) {
            auto res = dir->mkdir("sub1/sub2", Directory::Recursive::Yes);
            if (!res) {
                fmt::print(stderr, "Flippy: cannot create directory: {}\n", res.error().message());
                //return 1;
            }
        }

        auto maybeentries = dir->dir();
        if (!maybeentries.has_value()) {
            fmt::print(stderr, "Flippy: cannot list entries: {}\n", maybeentries.error().message());
            return 1;
        }
        for (auto& entry : maybeentries.value()) {
            if (entry.isFile()) {
                print_indented(indent, "File '{}'\n", entry.asFile()->longPath().value());
            } else if (entry.isDirectory()) {
                print_indented(indent, "Directory '{}'\n", entry.asDirectory()->longPath().value());
                indent += 2;
                printDir(entry.acquireDirectory());
                indent -= 2;
            }
        }

        return 0;
    };

    return printDir(rootdir);
}
