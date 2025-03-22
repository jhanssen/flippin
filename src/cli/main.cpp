#include "Args.h"
#include <Filesystem.h>
#include <fmt/format.h>

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
    const auto& imageName = args.freeformValue(0);
    auto fs = Filesystem::root(imageName);
    if (!fs) {
        fmt::print(stderr, "Flippy: cannot open image '{}': {}\n", imageName, fs.error().message());
        return 1;
    }
    return 0;
}
