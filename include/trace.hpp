#ifndef TRACE_HPP
#define TRACE_HPP

#include <cstdint>

class TraceStat {
public:
    TraceStat();
    ~TraceStat();

private:
    std::int_fast64_t comparison;
    std::int_fast64_t swap;
};

template<typename T>
class Trace {
};

#endif