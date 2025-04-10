#ifndef TRACE_HPP
#define TRACE_HPP

#include <cstdint>

class Trace {
public:
    Trace() {}

    template<std::int_fast64_t Diff>
    inline void access()
    { cnt_access += Diff; }

    template<std::int_fast64_t Diff>
    inline void comp()
    { cnt_comp += Diff; }

    inline std::int_fast64_t count_access() const
    { return cnt_access; }

    inline std::int_fast64_t count_comp() const
    { return cnt_comp; }

private:
    std::int_fast64_t cnt_access = 0;
    std::int_fast64_t cnt_comp = 0;
    std::int_fast64_t cnt_swap = 0;
};

#endif