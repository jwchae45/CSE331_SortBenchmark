#ifndef SORTBASE_HPP
#define SORTBASE_HPP

#include <algorithm>

#include "trace.hpp"
#include "filesys.hpp"

class SortBase {
public:
    SortBase(Mount& _mnt) : mnt(_mnt) {}

    template<class IntType>
    inline IntType& at(std::size_t _idx)
    { tr.access<1>(); return mnt.at<IntType>(_idx); }

    template<class IntType>
    inline void set(std::size_t _idx1, std::size_t _idx2)
    { tr.access<2>(); mnt.at<IntType>(_idx1) = mnt.at<IntType>(_idx2); }

    template<class IntType>
    inline void set_val(std::size_t _idx, const IntType& _val)
    { tr.access<1>(); mnt.at<IntType>(_idx) = _val; }

    template<class IntType>
    inline void set_direct(IntType& _val1, const IntType& _val2)
    { tr.access<1>(); _val1 = _val2; }

    template<class IntType>
    inline bool lte(std::size_t _idx1, std::size_t _idx2)
    { tr.comp<1>(); tr.access<2>(); return mnt.at<IntType>(_idx1) <= mnt.at<IntType>(_idx2); }

    template<class IntType>
    inline bool lte_direct(const IntType& _i1, const IntType& _i2)
    { tr.comp<1>(); return _i1 <= _i2; }

    template<class IntType>
    inline bool lt(std::size_t _idx1, std::size_t _idx2)
    { tr.comp<1>(); tr.access<2>(); return mnt.at<IntType>(_idx1) < mnt.at<IntType>(_idx2); }

    template<class IntType>
    inline bool lt_direct(const IntType& _i1, const IntType& _i2)
    { tr.comp<1>(); return _i1 < _i2; }

    template<class IntType>
    inline bool gte(std::size_t _idx1, std::size_t _idx2)
    { tr.comp<1>(); tr.access<2>(); return mnt.at<IntType>(_idx1) >= mnt.at<IntType>(_idx2); }

    template<class IntType>
    inline bool gte_direct(const IntType& _i1, const IntType& _i2)
    { tr.comp<1>(); return _i1 >= _i2; }

    template<class IntType>
    inline bool gt(std::size_t _idx1, std::size_t _idx2)
    { tr.comp<1>(); tr.access<2>(); return mnt.at<IntType>(_idx1) > mnt.at<IntType>(_idx2); }

    template<class IntType>
    inline bool gt_direct(const IntType& _i1, const IntType& _i2)
    { tr.comp<1>(); return _i1 > _i2; }

    template<class IntType>
    inline void swap(std::size_t _idx1, std::size_t _idx2) {
        tr.access<3>();
        IntType& a = mnt.at<IntType>(_idx1);
        IntType& b = mnt.at<IntType>(_idx2);
        IntType t = a; a = b; b = t;
    }

    template<class IntType>
    std::size_t size(void) const
    { return mnt.size<IntType>(); }

    inline const Trace& trace(void) const
    { return tr; }

    inline bool validate(bool verbose = false) const
    { return mnt.validate(verbose); }

    virtual void run(void) = 0;

    template<std::int_fast64_t Diff>
    inline void manual_access()
    { tr.access<Diff>(); }

    template<std::int_fast64_t Diff>
    inline void manual_comp()
    { tr.comp<Diff>(); }

protected:
    Mount& mnt;
    Trace tr;
};

#endif