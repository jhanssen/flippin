#include "FileIO.h"

#include <cstdio>

namespace flippy {
namespace fileio {

Result<std::vector<uint8_t>> read(const std::filesystem::path& path)
{
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) {
        return std::unexpected(Error("cannot open file for read: {}", path));
    }

    // get file size
    fseek(f, 0, SEEK_END);
    const long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t buffer[65536];
    std::vector<uint8_t> data;
    data.reserve(fsize);
    while (!feof(f)) {
        const size_t read = fread(buffer, 1, sizeof(buffer), f);
        if (read == 0) {
            fclose(f);
            return std::unexpected(Error("error reading file: {}", path));
        }
        data.insert(data.end(), buffer, buffer + read);
    }

    fclose(f);

    return data;
}

Result<void> write(const std::filesystem::path& path, const std::vector<uint8_t>& data)
{
    FILE* f = fopen(path.c_str(), "wb");
    if (!f) {
        return std::unexpected(Error("cannot open file for write: {}", path));
    }

    auto rem = data.size();
    auto off = 0;
    while (rem > 0) {
        const std::size_t towrite = std::min<std::size_t>(rem, 65536);
        const std::size_t written = fwrite(data.data() + off, 1, towrite, f);
        if (written != towrite) {
            fclose(f);
            return std::unexpected(Error("short write: {} ({} < {})", path, written, towrite));
        }
        rem -= written;
        off += written;
    }

    fclose(f);

    return {};
}

}} // namespace flippy::file
