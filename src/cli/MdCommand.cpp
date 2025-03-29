#include "MdCommand.h"
#include "Options.h"
#include <Directory.h>

using namespace flippin;

int MdCommand::execute(std::filesystem::path path, Format format, std::vector<std::string> cargs)
{
    const int fexecute = FilesystemCommand::execute(std::move(path), format, std::move(cargs));
    if (fexecute != 0) {
        return fexecute;
    }

    const auto& a = args();
    const Options opts(a);

    if (opts.nonOptions() != 1) {
        fmt::print(stderr, "{}: requires one argument\n", name());
        return 1;
    }

    const auto recursive = opts.has('p') ? Directory::Recursive::Yes : Directory::Recursive::No;

    const auto& dir = directory();
    auto res = dir->md(a[opts.nonOption(0)], recursive);
    if (!res) {
        fmt::print(stderr, "{}: cannot md directory: {}\n", name(), res.error().message());
        return 1;
    }

    fmt::print("{}: made directory, {}\n", name(), a[opts.nonOption(0)]);

    return 0;
}

std::vector<std::string> MdCommand::syntax() const
{
    return {
        "md [-p] <source>",
        "  Makes a directory inside of the image"
    };
}
