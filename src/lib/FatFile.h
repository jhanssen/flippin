#pragma once

#include "FatFat.h"
#include "File.h"
#include <memory>
extern "C" {
#include <llfat.h>
}

namespace flippy {

class FatDirectory;

class FatFile : public File
{
public:
    virtual ~FatFile() override;

    virtual Result<std::filesystem::path> path() const override;
    virtual Result<std::size_t> size() const override;
    virtual Result<std::vector<uint8_t>> read(std::size_t offset = 0, std::size_t size = std::numeric_limits<std::size_t>::max()) const override;
    virtual Result<std::size_t> write(std::size_t offset, const std::vector<uint8_t>& data) override;
    virtual Result<void> close() override;

private:
    FatFile(std::shared_ptr<FatFat> fat, const std::filesystem::path& path, unit* directory, int index);

private:
    std::shared_ptr<FatFat> mFat;
    std::filesystem::path mPath;
    unit* mDirectory = nullptr;
    int mIndex = 0;

    friend class FatDirectory;
};

} // namespace flippy
