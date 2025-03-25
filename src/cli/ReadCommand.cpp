#include "ReadCommand.h"
#include <Directory.h>
#include <File.h>
#include <FileIO.h>

using namespace flippy;

int ReadCommand::execute(std::filesystem::path path, Format format, std::vector<std::string> cargs)
{
    const int fexecute = FilesystemCommand::execute(std::move(path), format, std::move(cargs));
    if (fexecute != 0) {
        return fexecute;
    }

    const auto& a = args();
    if (a.size() != 2) {
        fmt::print(stderr, "{}: requires two arguments\n", name());
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

    auto maybeok = fileio::write(a[1], contents);
    if (!maybeok.has_value()) {
        fmt::print(stderr, "{}: cannot write host file: {}\n", name(), maybeok.error().message());
        return 1;
    }

    fmt::print("{}: read file, {} -> {}\n", name(), a[0], a[1]);

    return 0;
}

std::vector<std::string> ReadCommand::syntax() const
{
    return {
        "read <source> <destination>",
        "  Transfers a file from an image to the host"
    };
}
