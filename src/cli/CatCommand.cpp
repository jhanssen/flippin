#include "CatCommand.h"
#include <Directory.h>
#include <File.h>

using namespace flippy;

int CatCommand::execute(std::filesystem::path path, Format format, std::vector<std::string> cargs)
{
    const int fexecute = FilesystemCommand::execute(std::move(path), format, std::move(cargs));
    if (fexecute != 0) {
        return fexecute;
    }

    const auto& a = args();
    if (a.size() != 1) {
        fmt::print(stderr, "{}: requires one argument\n", name());
        return 1;
    }

    const auto& dir = directory();
    auto maybesrc = dir->openFile(a[0], Directory::OpenFileMode::Read);
    if (!maybesrc.has_value()) {
        fmt::print(stderr, "{}: cannot open image file: {}\n", name(), maybesrc.error().message());
        return 1;
    }
    auto src = std::move(maybesrc).value();

    auto maybecontents = src->read();
    if (!maybecontents.has_value()) {
        fmt::print(stderr, "{}: cannot read image file: {}\n", name(), maybecontents.error().message());
        return 1;
    }
    auto contents = std::move(maybecontents).value();
    const std::string str(contents.begin(), contents.end());
    fmt::print("{}\n", str);

    return 0;
}

std::vector<std::string> CatCommand::syntax() const
{
    return {
        "cat <source>",
        "  Cats a file inside of the image"
    };
}
