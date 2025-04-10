#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <ostream>
#include <chrono>

#include "afx/trace.hpp"
#include "afx/dataset.hpp"

class BenchResult {
public:
    using duration_t = std::chrono::milliseconds;

    BenchResult(const TraceStat& _trace, const duration_t& _duration) : trace(_trace), duration(_duration) {}
    friend std::ostream& operator<<(std::ostream&, const BenchResult& bres);

private:
    const TraceStat trace;
    const duration_t duration;
};

template<class SortingMethod>
BenchResult benchmark(const Dataset& dataset, bool verbose = false);

#include "benchmark.tpp"

#endif