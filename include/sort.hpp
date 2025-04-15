#ifndef SORT_HPP
#define SORT_HPP

#include <algorithm> // std::swap
#include <utility> // std::pair

#include "sortbase.hpp"
#include "filesys.hpp"

#define UNUSED(X) (void)(X)

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
                    swap<IntType>(i, i + 1);
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

class Library : public SortBase {
private:
    static constexpr std::size_t spreading_factor = 3;

public:
    Library(Mount& _mnt) : SortBase(_mnt) {}

    template<class IntType>
    void Rebalance(std::vector<IntType>& S, std::size_t total) {
        std::size_t step_size = S.size() / (total+1);

        static std::vector<IntType> T(size<IntType>());
        std::size_t i, k = 0;
        for (i = 0; i < S.size(); ++i) {
            if (S[i]) { tr.access<3>();
                T[k++] = S[i];
                S[i] = 0;
            }
        }

        i = 0;
        for (std::size_t w = step_size; i < k; w += step_size) {
            S[w] = T[i++]; tr.access<2>();
        }
    }

    template<class IntType>
    std::size_t BinarySearch(const std::vector<std::pair<IntType, std::size_t>>& sorted, IntType val) {
        std::size_t low = 0, mid, high = sorted.size();
        while (low < high) {
            mid = (low + high) / 2;
            tr.access<1>();
            tr.comp<1>();
            if (sorted[mid].first <= val) low = mid + 1;
            else                          high = mid;
        }
        return sorted[!low ? 0 : low - 1].second;
    }

    template<class IntType>
    void run_(void) {
        std::size_t N = size<IntType>();
        std::size_t S_size = N * 4; // epsilon=1 => 2*(1+epsilon)=4

        std::vector<IntType> S(S_size); // [!] gap = 0, needs to assure there's no 0 in data

        std::size_t mid = S_size / 2;
        S[mid] = at<IntType>(0);

        std::size_t total = 1;
        std::size_t round = 1;
        std::size_t insertion;

        IntType val;

        while (total < N) {
            insertion = std::min(N - total, (round <<= 1));
            Rebalance(S, total);
            std::vector<std::pair<IntType, std::size_t>> sorted(total);
            for (size_t i = 0, k = 0; i < S_size && k < total; ++i) {
                tr.access<1>();
                if (S[i]) {
                    tr.access<1>();
                    sorted[k++] = std::make_pair(S[i], i);
                }
            }

            bool retry = false;
            for (std::size_t j = total; j < total + insertion; ++j) {
                if (!retry) [[likely]] val = at<IntType>(j);
                else {
                    retry = false;
                    Rebalance(S, --j);
                    sorted.clear();
                    for (size_t i = 0; i < S_size; ++i) {
                        tr.access<1>();
                        if (S[i]) {
                            tr.access<1>();
                            sorted.push_back(std::make_pair(S[i], i));
                        }
                    }
                }
                std::size_t idx = BinarySearch(sorted, val); // S[idx] <= val

                if (S[idx] <= val) { // insert right
                    while (true) {
                        if (++idx > S_size) {
                            retry = true;
                            break;
                        }
                        IntType& occupied = S[++idx]; tr.access<1>();
                        if (!occupied) break;
                        tr.comp<1>();
                        if (val < occupied) { std::swap(occupied, val); tr.access<3>(); }
                    }
                } else { // insert left
                    while (true) {
                        if (idx-- == 0) {
                            retry = true;
                            break;
                        }
                        IntType& occupied = S[--idx]; tr.access<1>();
                        if (!occupied) break;
                        tr.comp<1>();
                        if (val >= occupied) { std::swap(occupied, val); tr.access<3>(); }
                        // (val >  occupied) leads to unstable sort
                    }
                }
                if (retry) [[unlikely]] continue;
                S[idx] = val; tr.access<1>();
            }
            total += insertion;
        }

        for (std::size_t i = 0, k = 0; i < S_size && k < N; ++i) {
            tr.access<1>();
            if (!S[i]) continue;
            set_val<IntType>(k++, S[i]); tr.access<1>();
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

class Cocktail : public SortBase {
public:
    Cocktail(Mount& _mnt) : SortBase(_mnt) {}

    template<class IntType>
    void run_(void) {
        std::size_t begin = 0, end = size<IntType>(); // [begin, end)
        
        bool swapped;
        while (true) {
            if (end - begin < 1) break;

            // forward
            swapped = false;
            for (std::size_t i = begin; i < end - 1; ++i) {
                if (!lte<IntType>(i, i + 1)) {
                    swap<IntType>(i, i + 1);
                    swapped = true;
                }
            }
            if (!swapped) break;
            swapped = false;
            --end;
            
            if (end - begin < 1) break;

            // backward
            for (std::size_t i = end; i-- > begin;) {
                if (!lte<IntType>(i, i + 1)) {
                    swap<IntType>(i, i + 1);
                    swapped = true;
                }
            }
            if (!swapped) break;
            ++begin;
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

class Comb : public SortBase {
private:
    static constexpr double shrink_factor = 1.3;

public:
    Comb(Mount& _mnt) : SortBase(_mnt) {}

    template<class IntType>
    void run_(void) {
        // for ℓ := 1 to t do
        // begin
        //     inc := h_ℓ;
        //     for i := 1 to n - inc do
        //         if A[i] > A[i + inc] then
        //             A[i] ↔ A[i + inc];
        // end;
        // ℓ := n - 1;
        // while ℓ > 0 do
        // begin
        //     k := 0;
        //     for i := 1 to ℓ do
        //         if A[i] > A[i + 1] then
        //             {A[i] ↔ A[i + 1]; k := i};
        //     ℓ := k - 1;
        // end;
        std::size_t N = size<IntType>();
        std::size_t i, inc = N;
        while ((inc = static_cast<std::size_t>(inc / 1.3)) > 1) {
            for (i = 0; i < N - inc; ++i) {
                if (!lte<IntType>(i, i + inc))
                    swap<IntType>(i, i + inc);
            }
        }
        // inc = 1 (bubble)
        bool swapped = true;
        while (swapped) {
            swapped = false;
            for (i = 0; i < N - 1; ++i) {
                if (!lte<IntType>(i, i + 1)) {
                    swap<IntType>(i, i + 1);
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

class Tournament : public SortBase {
private:
    static constexpr std::size_t INF = (std::size_t)(-1);

public:
    Tournament(Mount& _mnt) : SortBase(_mnt) {}

    template<class IntType>
    std::size_t Winner(std::size_t a, std::size_t b) {
        if (a == INF) return b;
        if (b == INF) return a;

        IntType va = at<IntType>(a);
        IntType vb = at<IntType>(b);

        return (lte_direct<IntType>(va, vb) ? a : b);
    }

    template<class IntType>
    void run_(void) {
        std::size_t N = size<IntType>();
        std::vector<std::size_t> T(2*N, INF);
        std::vector<std::size_t> R(N);

        std::size_t i, j;
        for (i = 0; i < N; ++i) {
            T[N + i] = i;              tr.access<1>();
        }

        for (i = N - 1; i > 0; --i) {
                                       tr.access<3>();
            T[i] = Winner<IntType>(T[i << 1], T[(i << 1) + 1]);
        }
    
        for (j = 0; j < N; ++j) {
            std::size_t min = T[1];    tr.access<1>();
            R[j] = at<IntType>(min);   tr.access<1>();
            i = N + min;
            T[i] = INF;                tr.access<1>();
            while (i >>= 1) {
                                       tr.access<1>();
                T[i] = Winner<IntType>(T[i << 1], T[(i << 1) + 1]);
            }
        }
        for (j = 0; j < N; ++j) {
            set_val<IntType>(j, R[j]); tr.access<1>();
        }
    }

    void run(void) {
        switch (mnt.meta.bsize) {
        case 8:  run_<std::uint8_t>(); break;
        case 16: run_<std::uint16_t>(); break;
        case 32: run_<std::uint32_t>(); break;
        case 64: run_<std::uint64_t>(); break;
        }
    }
};

class Introsort : public SortBase {
public:
    Introsort(Mount& _mnt) : SortBase(_mnt) {}

    template<class IntType>
    void SwopHeap(std::size_t low, std::size_t high, IntType in, IntType& out) {
        if (gte_direct<IntType>(in, at<IntType>(low))) {
            set_direct<IntType>(out, in);
            return;
        }

        std::size_t i = low;
        set_direct<IntType>(out, at<IntType>(low));
        while (true) {
            std::size_t j = 2 * (i - low) + 1 + low;
            if (j >= high)
                break;
            if (j + 1 < high && gt<IntType>(j + 1, j))
                ++j;
            if (gt_direct<IntType>(at<IntType>(j), in)) {
                set<IntType>(i, j);
                i = j;
            } else break;
        }
        set_val<IntType>(i, in);
    }

    template<class IntType>
    void InHeap(std::size_t low, std::size_t high, std::size_t index, IntType in) {
        UNUSED(high);
        std::size_t i = index;
        while (i > low) {
            std::size_t j = ((i - low) - 1) / 2 + low;
            if (gt_direct<IntType>(in, at<IntType>(j))) {
                set<IntType>(i, j);
                i = j;
            } else break;
        }
        set_val<IntType>(i, in);
    }

    template<class IntType>
    void OutHeap(std::size_t low, std::size_t high, IntType& out) {
        SwopHeap<IntType>(low, high, at<IntType>(high - 1), out);
    }

    template<class IntType>
    void SetHeap(std::size_t low, std::size_t high) {
        for (std::size_t j = low + 1; j < high; ++j)
            InHeap<IntType>(low, high, j, at<IntType>(j));
    }

    template<class IntType>
    void HeapSort(std::size_t low, std::size_t high) {
        IntType out;
        SetHeap<IntType>(low, high); // in-place rearrangement
        for (std::size_t i = high - 1; i > low; --i) {
            OutHeap<IntType>(low, high, out);
            set_val<IntType>(i, out);
            --high;
        }
    }

    template<class IntType>
    void InsertionSort(std::size_t low, std::size_t high) {
        for (std::size_t i = low + 1; i < high; ++i) {
            for (std::size_t j = i; j > low && lte<IntType>(j, j - 1); --j)
                swap<IntType>(j, j - 1);
        }
    }

    template<class IntType>
    IntType Median(const IntType& a, const IntType& b, const IntType& c) {
        if ((lte_direct<IntType>(a, b) && lte_direct<IntType>(b, c)) ||
            (lte_direct<IntType>(c, b) && lte_direct<IntType>(b, a))) return b;
        if ((lte_direct<IntType>(b, a) && lte_direct<IntType>(a, c)) ||
            (lte_direct<IntType>(c, a) && lte_direct<IntType>(a, b))) return a;
        return c;
    }

    template<class IntType>
    void IntroLoop(std::size_t low, std::size_t high, std::size_t depth) {
        std::size_t i, j, mid;
        while (high - low > 16) {
            if (depth-- == 0) {
                HeapSort<IntType>(low, high);
                return;
            } else {
                IntType pivot = Median(at<IntType>(low), at<IntType>((low + high - 1) / 2), at<IntType>(high - 1));
                i = low - 1, j = high;
                while (true) {
                    do { --j; } while (gt_direct<IntType>(at<IntType>(j), pivot));
                    do { ++i; } while (lt_direct<IntType>(at<IntType>(i), pivot));
                    if (i < j) swap<IntType>(i, j);
                    else break;
                }
                mid = j + 1;
                IntroLoop<IntType>(mid, high, depth);
                high = mid; //tail-recursion
            }
        }
        InsertionSort<IntType>(low, high);
    }

    std::size_t log2(std::size_t n) {
        std::size_t r = 0;
        while (n >>= 1) ++r;
        return r;
    }

    template<class IntType>
    void run_(void) {
        std::size_t N = size<IntType>();
        std::size_t depth = 2 * log2(N);
        IntroLoop<IntType>(0, N, depth);
    }

    void run(void) {
        switch (mnt.meta.bsize) {
        case 8:  run_<std::uint8_t>(); break;
        case 16: run_<std::uint16_t>(); break;
        case 32: run_<std::uint32_t>(); break;
        case 64: run_<std::uint64_t>(); break;
        }
    }
};

class Tim : public SortBase {
private:
    static constexpr std::size_t MIN_MERGE = 32;

public:
    Tim(Mount& _mnt) : SortBase(_mnt) {}

    template<class IntType>
    std::size_t FindRun(std::size_t begin, std::size_t N) {
        std::size_t i = begin + 1;
        if (i == N) return N;
        if (lt<IntType>(i, i - 1)) { // descending
            while (i < N && lt<IntType>(i, i - 1)) ++i;
            ReverseRun<IntType>(begin, i);
        } else { // ascending
            while (i < N && !lt<IntType>(i, i - 1)) ++i;
        }
        return i;
    }

    template<class IntType>
    void ReverseRun(std::size_t low, std::size_t high) {
        while (low < high - 1) {
            swap<IntType>(low++, --high);
        }
    }

    template<class IntType>
    void InsertionSort(std::size_t low, std::size_t high) {
        for (std::size_t i = low + 1; i < high; ++i) {
            for (std::size_t j = i; j > low && lte<IntType>(j, j - 1); --j)
                swap<IntType>(j, j - 1);
        }
    }

    template<class IntType>
    void Merge(std::size_t low, std::size_t mid, std::size_t high) {
        std::vector<IntType> left(mid - low);
        for (std::size_t i = 0; i < left.size(); ++i)
            left[i] = at<IntType>(low + i);

        std::size_t i = 0, j = mid, k = low;
        while (i < left.size() && j < high) {
            if (lte_direct<IntType>(left[i], at<IntType>(j)))
                set_val<IntType>(k++, left[i++]);
            else
                set_val<IntType>(k++, at<IntType>(j++));
        }
        while (i < left.size())
            set_val<IntType>(k++, left[i++]);
    }

    template<class IntType>
    void MergeCollapse(std::vector<std::pair<std::size_t, std::size_t>>& stack) {
        while (stack.size() >= 2) {
            std::size_t n = stack.size();
            std::pair<std::size_t, std::size_t> A = stack[n - 1];
            std::pair<std::size_t, std::size_t> B = stack[n - 2];
            std::pair<std::size_t, std::size_t> C;
            std::size_t A_low = A.first, A_high = A.second;
            std::size_t B_low = B.first, B_high = B.second;
            std::size_t C_low, C_high;

            std::size_t A_len = A_high - A_low;
            std::size_t B_len = B_high - B_low;

            if (n >= 3) {
                C = stack[n - 3];
                C_low = C.first; C_high = C.second;
                std::size_t C_len = C_high - C_low;

                if (C_len <= B_len + A_len || B_len <= A_len) {
                    if (C_len < A_len) {
                        Merge<IntType>(C_low, B_low, B_high);
                        stack[n - 3].second = B_high;
                        stack.erase(stack.begin() + (n - 2));
                    } else {
                        Merge<IntType>(B_low, A_low, A_high);
                        stack[n - 2].second = A_high;
                        stack.pop_back();
                    }
                    continue;
                }
            }

            if (B_len <= A_len) {
                Merge<IntType>(B_low, A_low, A_high);
                stack[n - 2].second = A_high;
                stack.pop_back();
            } else {
                break;
            }
        }
    }

    template<class IntType>
    void MergeForceCollapse(std::vector<std::pair<std::size_t, std::size_t>>& stack) {
        while (stack.size() > 1) {
            std::pair<std::size_t, std::size_t> A = stack.back(); stack.pop_back();
            std::pair<std::size_t, std::size_t> B = stack.back(); stack.pop_back();
            Merge<IntType>(B.first, A.first, A.second);
            stack.emplace_back(B.first, A.second);
        }
    }

    std::size_t CalcMinRun(std::size_t n) {
        std::size_t r = 0;
        while (n >= MIN_MERGE) {
            r |= (n & 1);
            n >>= 1;
        }
        return n + r;
    }

    template<class IntType>
    void run_() {
        std::size_t N = size<IntType>();
        std::size_t minrun = CalcMinRun(N);

        std::vector<std::pair<std::size_t, std::size_t>> run_stack;
        std::size_t i = 0;

        while (i < N) {
            std::size_t run_begin = i;
            std::size_t run_end = FindRun<IntType>(i, N);

            std::size_t run_len = run_end - run_begin;
            if (run_len < minrun) {
                std::size_t forced_end = std::min(N, run_begin + minrun);
                InsertionSort<IntType>(run_begin, forced_end);
                run_end = forced_end;
            }

            run_stack.emplace_back(run_begin, run_end);
            MergeCollapse<IntType>(run_stack);

            i = run_end;
        }

        MergeForceCollapse<IntType>(run_stack);
    }

    void run(void) {
        switch (mnt.meta.bsize) {
        case 8:  run_<std::uint8_t>(); break;
        case 16: run_<std::uint16_t>(); break;
        case 32: run_<std::uint32_t>(); break;
        case 64: run_<std::uint64_t>(); break;
        }
    }
};

// Library Sort 기반, Rebalancing 단계에 data distribution을 추론하는 과정을 넣어 nearest gap 까지의 distance를 최소화!
// Samples = [5, 14, 3] in domain [0, 15)
// 3, 5 주변에 데이터가 밀집되어 있다고 판단하자 (LLN에 근거한 Inferrence)
// S = [_, _, _, _, _, 3, _, _, _, _, _, 5, _, _, _, 14, _, _, _]
//
// class Infer : public SortBase {
// private:
//     static constexpr std::size_t spreading_factor = 3;

// public:
//     Infer(Mount& _mnt) : SortBase(_mnt) {}

//     template<class IntType>
//     void Rebalance(std::vector<IntType>& S, std::size_t total) {
//         std::size_t step_size = S.size() / (total+1);

//         static std::vector<IntType> T(size<IntType>());
//         std::size_t i, k = 0;
//         for (i = 0; i < S.size(); ++i) {
//             if (S[i]) { tr.access<3>();
//                 T[k++] = S[i];
//                 S[i] = 0;
//             }
//         }

//         i = 0;
//         for (std::size_t w = step_size; i < k; w += step_size) {
//             S[w] = T[i++]; tr.access<2>();
//         }
//     }

//     template<class IntType>
//     std::size_t BinarySearch(const std::vector<std::pair<IntType, std::size_t>>& sorted, IntType val) {
//         std::size_t low = 0, mid, high = sorted.size();
//         while (low < high) {
//             mid = (low + high) / 2;
//             tr.access<1>();
//             tr.comp<1>();
//             if (sorted[mid].first <= val) low = mid + 1;
//             else                          high = mid;
//         }
//         return sorted[!low ? 0 : low - 1].second;
//     }

//     template<class IntType>
//     void run_(void) {
//         std::size_t N = size<IntType>();
//         std::size_t S_size = N * 4; // epsilon=1 => 2*(1+epsilon)=4

//         std::vector<IntType> S(S_size); // [!] gap = 0, needs to assure there's no 0 in data

//         std::size_t mid = S_size / 2;
//         S[mid] = at<IntType>(0);

//         std::size_t total = 1;
//         std::size_t round = 1;
//         std::size_t insertion;

//         IntType val;

//         while (total < N) {
//             insertion = std::min(N - total, (round <<= 1));
//             Rebalance(S, total);
//             std::vector<std::pair<IntType, std::size_t>> sorted(total);
//             for (size_t i = 0, k = 0; i < S_size && k < total; ++i) {
//                 tr.access<1>();
//                 if (S[i]) {
//                     tr.access<1>();
//                     sorted[k++] = std::make_pair(S[i], i);
//                 }
//             }

//             bool retry = false;
//             for (std::size_t j = total; j < total + insertion; ++j) {
//                 if (!retry) [[likely]] val = at<IntType>(j);
//                 else {
//                     retry = false;
//                     Rebalance(S, --j);
//                     sorted.clear();
//                     for (size_t i = 0; i < S_size; ++i) {
//                         tr.access<1>();
//                         if (S[i]) {
//                             tr.access<1>();
//                             sorted.push_back(std::make_pair(S[i], i));
//                         }
//                     }
//                 }
//                 std::size_t idx = BinarySearch(sorted, val); // S[idx] <= val

//                 if (S[idx] <= val) { // insert right
//                     while (true) {
//                         if (++idx > S_size) {
//                             retry = true;
//                             break;
//                         }
//                         IntType& occupied = S[++idx]; tr.access<1>();
//                         if (!occupied) break;
//                         tr.comp<1>();
//                         if (val < occupied) { std::swap(occupied, val); tr.access<3>(); }
//                     }
//                 } else { // insert left
//                     while (true) {
//                         if (idx-- == 0) {
//                             retry = true;
//                             break;
//                         }
//                         IntType& occupied = S[--idx]; tr.access<1>();
//                         if (!occupied) break;
//                         tr.comp<1>();
//                         if (val >= occupied) { std::swap(occupied, val); tr.access<3>(); }
//                         // (val >  occupied) leads to unstable sort
//                     }
//                 }
//                 if (retry) [[unlikely]] continue;
//                 S[idx] = val; tr.access<1>();
//             }
//             total += insertion;
//         }

//         for (std::size_t i = 0, k = 0; i < S_size && k < N; ++i) {
//             tr.access<1>();
//             if (!S[i]) continue;
//             set_val<IntType>(k++, S[i]); tr.access<1>();
//         }
//     }

//     void run(void) {
//         switch (mnt.meta.bsize) {
//         case 8:  run_<std::uint8_t >(); break;
//         case 16: run_<std::uint16_t>(); break;
//         case 32: run_<std::uint32_t>(); break;
//         case 64: run_<std::uint64_t>(); break;
//         }
//     }
// };

#endif