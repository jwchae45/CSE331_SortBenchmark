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
                if (!lte<IntType>(i, i + 1)) {
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
                if (!lte_direct<IntType>(min, arr_j)) {
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
            for (j = i; j > 0 && lte<IntType>(j, j-1); --j)
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
                if (lte<IntType>(i, j)) {
                    // N4
                    set<IntType>(k, i);
                    if (d) ++k;
                    else   --k;
                    // N5
                    ++i;
                    if (lte<IntType>(i-1, i)) continue;
                    do {
                        // N6
                        set<IntType>(k, j);
                        if (d) ++k;
                        else   --k;
                        // N7
                        --j;
                    } while (lte<IntType>(j+1, j));
                } else {
                    // N8
                    set<IntType>(k, j);
                    if (d) ++k;
                    else   --k;
                    // N9
                    --j;
                    if (lte<IntType>(j+1, j)) continue;
                    do {
                        // N10
                        set<IntType>(k, i);
                        if (d) ++k;
                        else   --k;
                        // N11
                        ++i;
                    } while (lte<IntType>(i-1, i));
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
            // sorting is ltelete
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

class Heap : public SortBase { // max-heap based heap sort
private:
    std::size_t n;

public:
    Heap(Mount& _mnt) : SortBase(_mnt) {}

    template<class IntType>
    void SwopHeap(IntType in, IntType& out) {
        if (gte_direct<IntType>(in, at<IntType>(0))) {
            set_direct<IntType>(out, in);
            return;
        }

        std::size_t i = 0;
        set_direct<IntType>(out, at<IntType>(0));
        while (true) {
            std::size_t j = 2 * i + 1;
            if (j >= n)
                break;
            if (j + 1 < n && gt<IntType>(j + 1, j))
                ++j;
            if (gt_direct<IntType>(at<IntType>(j), in)) {
                set<IntType>(i, j);
                i = j;
            } else break;
        }
        set_val<IntType>(i, in);
    }

    template<class IntType>
    void InHeap(std::size_t index, IntType in) {
        std::size_t i = index;
        while (i > 0) {
            std::size_t j = (i - 1) / 2;
            if (gt_direct<IntType>(in, at<IntType>(j))) {
                set<IntType>(i, j);
                i = j;
            } else break;
        }
        set_val<IntType>(i, in);
    }

    template<class IntType>
    void OutHeap(IntType& out) {
        SwopHeap<IntType>(at<IntType>(n - 1), out);
    }

    template<class IntType>
    void SetHeap() {
        for (std::size_t j = 1; j < n; ++j)
            InHeap<IntType>(j, at<IntType>(j));
    }

    template<class IntType>
    void run_() {
        IntType out;
        n = size<IntType>();
        SetHeap<IntType>(); // in-place rearrangement
        for (std::size_t i = n - 1; i > 0; --i) {
            OutHeap<IntType>(out);
            set_val<IntType>(i, out);
            --n;
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

class Quick : public SortBase { // Hoare partition
public:
    Quick(Mount& _mnt) : SortBase(_mnt) {}

    template<class IntType>
    void QuickSort(std::size_t M, std::size_t N) { // [M, N)
        if (M >= N - 1) return;
        // Partition(A, M, N, I, J)
        IntType pivot = at<IntType>(M);
        std::size_t I = M - 1;
        std::size_t J = N;
        while (true) {
            do {--J;} while (gt_direct<IntType>(at<IntType>(J), pivot));
            do {++I;} while (lt_direct<IntType>(at<IntType>(I), pivot));
            if (I < J) swap<IntType>(I, J);
            else break;
        }
        
        QuickSort<IntType>(M, J+1);
        QuickSort<IntType>(J+1, N);
    }

    template<class IntType>
    void run_(void) {
        QuickSort<IntType>(0, size<IntType>());
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

class QuickMid : public SortBase { // Pivot = mid of {A[M], A[(M+N)/2], A[N]}
public:
    QuickMid(Mount& _mnt) : SortBase(_mnt) {}

    template<class IntType>
    IntType Mid(const IntType& a, const IntType& b, const IntType& c) {
        if ((lte_direct<IntType>(b, a) && lte_direct<IntType>(a, c)) || (lte_direct<IntType>(c, a) && lte_direct<IntType>(a, b))) return a;
        if ((lte_direct<IntType>(a, b) && lte_direct<IntType>(b, c)) || (lte_direct<IntType>(c, b) && lte_direct<IntType>(b, a))) return b;
        return c;
    }

    template<class IntType>
    void QuickSort(std::size_t M, std::size_t N) { // [M, N)
        if (M >= N - 1) return;
        // Partition(A, M, N, I, J)
        IntType pivot = Mid(at<IntType>(M), at<IntType>((M+N-1)/2), at<IntType>(N - 1));
        std::size_t I = M - 1;
        std::size_t J = N;
        while (true) {
            do {--J;} while (gt_direct<IntType>(at<IntType>(J), pivot));
            do {++I;} while (lt_direct<IntType>(at<IntType>(I), pivot));
            if (I < J) swap<IntType>(I, J);
            else break;
        }
        
        QuickSort<IntType>(M, J+1);
        QuickSort<IntType>(J+1, N);
    }

    template<class IntType>
    void run_(void) {
        QuickSort<IntType>(0, size<IntType>());
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