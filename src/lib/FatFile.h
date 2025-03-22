#pragma once

#include "FatFat.h"
#include "File.h"
#include <memory>
#include <llfat.h>

namespace flippy {

class FatFile : public File
{
public:
    FatFile(std::shared_ptr<FatFat> fat, unit* directory, int index);
    virtual ~FatFile() override;

    virtual Result<std::size_t> size() const override;
    virtual Result<std::vector<uint8_t>> read(std::size_t offset = 0, std::size_t size = std::numeric_limits<std::size_t>::max()) const override;
    virtual Result<std::size_t> write(std::size_t offset, const std::vector<uint8_t>& data) override;
    virtual Result<void> close() override;

private:
    std::shared_ptr<FatFat> mFat;
    unit* mDirectory = nullptr;
    int mIndex = 0;
};

} // namespace flippy
