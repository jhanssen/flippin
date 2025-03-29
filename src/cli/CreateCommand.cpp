#include "CreateCommand.h"
#include <Filesystem.h>

using namespace flippin;

int CreateCommand::execute(std::filesystem::path path, Format format, std::vector<std::string> args)
{
    const auto& a = args;
    if (a.size() != 0) {
        fmt::print(stderr, "{}: accepts no argument\n", name());
        return 1;
    }

    auto maybedir = Filesystem::create(path, format);
    if (!maybedir.has_value()) {
        fmt::print(stderr, "{}: cannot create filesystem: {}\n", name(), maybedir.error().message());
        return 1;
    }

    fmt::print("{}: created filesystem\n", name());

    return 0;
}

std::vector<std::string> CreateCommand::syntax() const
{
    return {
        "create",
        "  Creates a new image file"
    };
}
