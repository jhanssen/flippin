#pragma once

#include <cstdlib>
extern "C" {
#include <llfat.h>
}

namespace flippin {

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

} // namespace flippin
