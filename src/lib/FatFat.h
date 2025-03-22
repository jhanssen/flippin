#pragma once

#include <llfat.h>
#include <cstdlib>

namespace flippy {

struct FatFat
{
    FatFat(fat* ff)
        : f(ff)
    {
    }
    ~FatFat() {
        if (f) {
            free(f->devicename);
            fatclose(f);
        }
    }

    fat* f;
};

} // namespace flippy
