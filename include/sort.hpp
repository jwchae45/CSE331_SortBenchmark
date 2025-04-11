#ifndef SORT_HPP
#define SORT_HPP

#include <exception>
#include <string>
#include <iostream>
#include <iomanip>

#include "sortbase.hpp"
#include "filesys.hpp"

class Bubble : public SortBase {
public:
    Bubble(Mount& _mnt) : SortBase(_mnt) {}

    template<class IntType>
    void run_(void) {
        for (std::size_t i = 0; i < mnt.meta.size; ++i) {
            for (std::size_t j = i; j < mnt.meta.size; ++j) {
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
};

class Selection : public SortBase {
public:
    Selection(Mount& _mnt) : SortBase(_mnt) {}

    template<class IntType>
    void run_(void) {
        IntType min, arr_j;
        std::size_t i, j, min_index;
        for (i = 0; i < mnt.meta.size; ++i) {
            min = at<IntType>(i);
            min_index = i;
            for (j = i + 1; j < mnt.meta.size; ++j) {
                arr_j = at<IntType>(j);
                if (!comp_direct<IntType>(min, arr_j)) {
                    min = arr_j;
                    min_index = j;
                }
            }
            swap<IntType>(i, min_index);
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
};

class Insertion : public SortBase {
public:
    Insertion(Mount& _mnt) : SortBase(_mnt) {}

    template<class IntType>
    void run_(void) {
        std::size_t i, j;
        for (i = 1; i < mnt.meta.size; ++i) {
            for (j = i; j > 0 && comp<IntType>(j, j-1); --j)
                swap<IntType>(j, j-1);
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
};

#endif