#include "Args.h"
#include <File.h>
#include <Filesystem.h>
#include <Directory.h>
#include <fmt/format.h>
#include <fmt/std.h>

using namespace flippy;

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
    printDir = [&printDir](const std::shared_ptr<Directory>& dir) -> int {
        auto maybeentries = dir->ls();
        if (!maybeentries.has_value()) {
            fmt::print(stderr, "Flippy: cannot list entries: {}\n", maybeentries.error().message());
            return 1;
        }
        for (auto& entry : maybeentries.value()) {
            if (entry.isFile()) {
                fmt::print("File '{}'\n", entry.asFile()->longPath().value());
            } else if (entry.isDirectory()) {
                fmt::print("Directory '{}'\n", entry.asDirectory()->longPath().value());
                printDir(entry.acquireDirectory());
            }
        }

        return 0;
    };

    return printDir(rootdir);
}
