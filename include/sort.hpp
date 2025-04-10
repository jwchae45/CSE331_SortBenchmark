#ifndef SORT_HPP
#define SORT_HPP

#include "sortbase.hpp"
#include "filesys.hpp"

class Bubble : public SortBase {
public:
    Bubble(Mount& _mnt) : SortBase(_mnt) {}

    template<class IntType>
    void run_(void) {
        for (std::int64_t i = 0; i < mnt.meta.size; ++i) {
            for (std::int64_t j = i; j < mnt.meta.size; ++j) {
                if (!comp<IntType>(i, j)) swap<IntType>(i, j);
            }
        }
    }

    void run(void) {
        switch (mnt.meta.bsize) {
        case 8:  run_<std::uint8_t >(); break;
        case 16: run_<std::uint16_t>(); break;
        case 32: run_<std::uint32_t>(); break;
        case 64: run_<std::uint64_t>(); break;
        }
    }

    bool validate(void) {
        return true;
    }
};

#endif