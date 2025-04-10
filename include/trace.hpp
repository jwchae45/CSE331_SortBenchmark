#ifndef TRACE_HPP
#define TRACE_HPP

#include <cstdint>

class Trace {
public:
    Trace() {}

    inline void access()
    { ++cnt_access; }

    inline void comp()
    { ++cnt_comp; }

    inline void swap()
    { ++cnt_swap; }

    inline std::int_fast64_t count_access() const
    { return cnt_access; }

    inline std::int_fast64_t count_comp() const
    { return cnt_comp; }

    inline std::int_fast64_t count_swap() const
    { return cnt_swap; }

private:
    std::int_fast64_t cnt_access = 0;
    std::int_fast64_t cnt_comp = 0;
    std::int_fast64_t cnt_swap = 0;
};

#endif