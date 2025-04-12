#ifndef SORT_HPP
#define SORT_HPP

#include <algorithm> // std::swap

#include "sortbase.hpp"
#include "filesys.hpp"

class Bubble : public SortBase {
public:
    Bubble(Mount& _mnt) : SortBase(_mnt) {}

    template<class IntType>
    void run_(void) {
        bool swapped = true;
        std::size_t i, N = size<IntType>();
        while (swapped) {
            swapped = false;
            for (i = 0; i < N - 1; ++i) {
                if (!comp<IntType>(i, i + 1)) {
                    swap<IntType>(i, i+1);
                    swapped = true;
                }
            }
            --N;
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

class Merge : public SortBase { // natural merge sort
public:
    Merge(Mount& _mnt) : SortBase(_mnt) {}

    template<class IntType>
    void run_(void) {
        std::size_t N = size<IntType>(), i, j, k, l;
        bool s, d, f;
        mnt.reserve(mnt.size<char>());
        // N1
        s = false; // copying direction
        while (true) {
            // N2
            if (!s) i = 0, j = N-1, k = N, l = 2*N - 1;
            else    k = 0, l = N-1, i = N, j = 2*N - 1;
            d = true; // current direction of output
            f = true; // zero(false) if future passes are necessary
            while (i != j) { // N3
                if (comp<IntType>(i, j)) {
                    // N4
                    set<IntType>(k, i);
                    if (d) ++k;
                    else   --k;
                    // N5
                    ++i;
                    if (comp<IntType>(i-1, i)) continue;
                    do {
                        // N6
                        set<IntType>(k, j);
                        if (d) ++k;
                        else   --k;
                        // N7
                        --j;
                    } while (comp<IntType>(j+1, j));
                } else {
                    // N8
                    set<IntType>(k, j);
                    if (d) ++k;
                    else   --k;
                    // N9
                    --j;
                    if (comp<IntType>(j+1, j)) continue;
                    do {
                        // N10
                        set<IntType>(k, i);
                        if (d) ++k;
                        else   --k;
                        // N11
                        ++i;
                    } while (comp<IntType>(i-1, i));
                }
                // N12
                f = false;
                d = !d;
                std::swap(k, l);
            }
            set<IntType>(k, i);
            // N13
            if (!f) {
                s = !s;
                continue;
            }
            // sorting is complete
            if (!s) for (i = 0, j = N; i < N;) set<IntType>(i++, j++);
            break;
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