#ifndef SORTBASE_HPP
#define SORTBASE_HPP

#include <algorithm>

#include "trace.hpp"
#include "filesys.hpp"

class SortBase {
public:
    SortBase(Mount& _mnt) : mnt(_mnt) {}

    template<class IntType>
    inline const IntType& at(std::size_t _idx)
    { tr.access(); return mnt.at<IntType>(_idx); }

    template<class IntType>
    inline void set(std::size_t _idx, IntType& _val)
    { tr.access(); mnt.at<IntType>(_idx) = _val; }

    template<class IntType>
    inline bool comp(std::size_t _idx1, std::size_t _idx2)
    { tr.comp(); return mnt.at<IntType>(_idx1) < mnt.at<IntType>(_idx2); }

    template<class IntType>
    inline void swap(std::size_t _idx1, std::size_t _idx2) {
        tr.swap();
        IntType& a = mnt.at<IntType>(_idx1);
        IntType& b = mnt.at<IntType>(_idx2);
        IntType t = a; a = b; b = t;
    }

    inline const Trace& trace(void) const
    { return tr; }

    virtual void run(void) = 0;
    virtual bool validate(void) = 0;

protected:
    Mount& mnt;
    Trace tr;
};

#endif