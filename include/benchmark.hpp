#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <ostream>
#include <memory>
#include <chrono>

#include "trace.hpp"
#include "sortbase.hpp"
#include "filesys.hpp"

template<class ClockResolution>
class BenchResult {
public:
    using duration_t = std::chrono::duration<double, ClockResolution>;

    BenchResult(const Trace& _trace, const duration_t& _duration) : trace(_trace), duration(_duration) {}

public:
    const Trace& trace;
    const duration_t duration;
};

using SortingMethod = std::unique_ptr<SortBase>;

template<class ClockResolution>
BenchResult<ClockResolution> benchmark(SortingMethod& sort) {
    auto begin = std::chrono::high_resolution_clock::now();
    sort->run();
    auto duration(std::chrono::high_resolution_clock::now() - begin);
    if (!sort->validate()) throw std::runtime_error("Sorted data do not match with the answer");
    return BenchResult<ClockResolution>(sort->trace(), duration);
}

#endif