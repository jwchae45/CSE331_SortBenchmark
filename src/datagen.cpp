#include <iostream>
#include <iomanip>
#include <format>
#include <cctype>
#include <cstdint>
#include <cmath>
#include <random>
#include <exception>

#include "argparse.hpp"
#include "filesys.hpp"

inline double NlogN(std::size_t N)
{ double n = static_cast<double>(N); return n * std::log2(n); }

template<class T>
void swap(std::mt19937& engine, std::vector<T>& list, std::size_t begin, std::size_t end) {
    std::uniform_int_distribution<std::size_t> dist(begin, end);
    std::swap(list[dist(engine)], list[dist(engine)]);
}

inline bool coin_flip(std::mt19937& engine)
{ return engine() % 2; }

template<class T>
void reverse_merge(std::mt19937& engine, std::vector<T>& list, std::size_t depth, std::size_t low = 0, std::size_t high = 0) {
    if (depth == 0) return;
    if (high == 0) high = list.size();
    std::size_t mid = (high + low) / 2;
    if (mid == low) return;
    std::vector<T> copy(high - low);
    std::size_t i, j = 0, k = mid - low;
    for (i = low; i < high; ++i) {
        if (coin_flip(engine)) copy[j++] = list[i];
        else                   copy[k++] = list[i];
        if (j == mid - low || k == high - low) break;
    }
    for (; j <  mid - low; ++j) copy[j] = list[++i];
    for (; k < high - low; ++k) copy[k] = list[++i];
    for (i = 0; i < high - low; ++i)
        list[low + i] = copy[i];
    reverse_merge(engine, list, depth - 1, low, mid);
    reverse_merge(engine, list, depth - 1, mid, high);
}

template<class IntType>
void generate(const std::size_t& iter,
              const std::string& dist,
              const std::string& pattern,
              std::mt19937& engine,
              const std::string& dest,
              bool verbose=false) {
    std::function<IntType()> distgen;
    bool randomly_generated = true;

    if (dist == "uniform") {
        // debug log: uniform_dist is a local instance, lambda captures dangling reference of uniform_dist, resulting to UB
        // (1) static std::uniform_int_distribution<IntType> uniform_dist <= ***given that generate() is called only once!***
        // (2) [uniform_dist, &engine] <= explicit capture
        static std::uniform_int_distribution<IntType> uniform_dist(std::numeric_limits<IntType>::min(), std::numeric_limits<IntType>::max());
        distgen = [&]() -> IntType { return uniform_dist(engine); };
    } else if (dist == "normal") {
        double mu = (std::numeric_limits<IntType>::max() + std::numeric_limits<IntType>::min()) / 2.;
        double sd = ((std::numeric_limits<IntType>::max() >> 1) / 4.) - ((std::numeric_limits<IntType>::min() >> 1) / 4.); // 4sigma ~ 99.994%
        static std::normal_distribution<double> normal_dist(mu, sd);
        distgen = [&]() {
            double x;
            double dmin = static_cast<double>(std::numeric_limits<IntType>::min());
            double dmax = static_cast<double>(std::numeric_limits<IntType>::max());
            while (true) {
                x = normal_dist(engine);
                if (dmin < x && x < dmax) break; // drop out-of-range values, range (min, max) is open for safety
            }
            return static_cast<IntType>(llround(x));
        };
    } else if (dist == "bimodal") {
        double mu = (std::numeric_limits<IntType>::max() + std::numeric_limits<IntType>::min()) / 2.;
        double sd = ((std::numeric_limits<IntType>::max() >> 2) / 4.) - ((std::numeric_limits<IntType>::min() >> 2) / 4.); // extra shifting (half domain)
        double mu1 = mu * (1. / 2);
        double mu2 = mu * (3. / 2);
        static std::normal_distribution<double> normal_dist1(mu1, sd);
        static std::normal_distribution<double> normal_dist2(mu2, sd);
        distgen = [&]() {
            double x;
            double dmin = static_cast<double>(std::numeric_limits<IntType>::min());
            double dmax = static_cast<double>(std::numeric_limits<IntType>::max());
            while (true) {
                if (coin_flip(engine)) x = normal_dist1(engine);
                else                   x = normal_dist2(engine);
                if (dmin < x && x < dmax) break; // drop out-of-range values, range (min, max) is open for safety
            }
            return static_cast<IntType>(llround(x));
        };
    } else { throw std::runtime_error("Unsupported distribution: " + dist); }
    
    if (verbose) std::cout << "Generating..."; // TODO: add progress bar
    Stream<IntType> unsorted(dest + ".unsorted");
    std::vector<IntType> list(iter);
    for (std::size_t i = 0; i < iter; ++i) list[i] = distgen();
    if (!randomly_generated) std::shuffle(list.begin(), list.end(), engine);
    if (verbose) std::cout << " [Done]\n";

    if (verbose) std::cout << "Fitting in " << pattern << " pattern...";
    if (pattern == "random") {
        // pass
    } else if (pattern == "almost") {
        std::sort(list.begin(), list.end());
        std::size_t n_swap = static_cast<std::size_t>(std::sqrt(list.size())); // N=1K => #swaps=32 | N=1M => #swaps=1K
        for (std::size_t i = 0; i < n_swap; ++i)
            swap(engine, list, 0, list.size());
    } else if (pattern == "noise") {
        std::sort(list.begin(), list.end());
        double k = std::log2(list.size() + 1);
        std::size_t window_size = static_cast<std::size_t>(k); // N=1K => window_size=10 | N=1M => window_size=20
        std::size_t n_swap = window_size / 3;                  //      => #swaps=3       |      => #swaps=6
        for (std::size_t w = 0; w < list.size() - window_size; ++w) {
            for (std::size_t i = 0; i < n_swap; ++i)
                swap(engine, list, w, w+window_size);
        }
    } else if (pattern == "reversed") {
        std::sort(list.begin(), list.end(), [](IntType _a, IntType _b) -> bool {
            return _a >= _b;
        });
    } else if (pattern == "sawtooth") {
        std::sort(list.begin(), list.end());
        std::size_t partition_depth = static_cast<std::size_t>(std::log2(list.size() + 1) / 3); // N=1K => depth=3 | N=1M => depth=6
        reverse_merge(engine, list, partition_depth);
    } else if (pattern == "bitonic") {
        std::size_t steps = 1 << static_cast<std::size_t>(std::log2(list.size() + 1) / 1.5); // N=1K => steps=64     | N=1M => steps=8192
        bool ascending = true;                                                               //      => partition=16 |      => partition=128
        for (std::size_t i = 0; i < list.size(); i += steps) {
            std::sort(list.begin() + i, list.begin() + i + steps,
                ascending ? [](IntType _a, IntType _b) { return _a <= _b; }
                          : [](IntType _a, IntType _b) { return _a >= _b; }
            );
            ascending = !ascending;
        }
    } else if (pattern == "frontsorted") {
        std::sort(list.begin(), list.begin() + list.size() / 2);
    } else if (pattern == "gap") {
        std::sort(list.begin(), list.end());
        std::shuffle(list.begin(), list.begin() + list.size() / 2, engine);
        std::shuffle(list.begin() + list.size() / 2, list.end(), engine);
        for (std::size_t i = 0, j = list.size() / 2; j < list.size(); i += 2, j += 2) std::swap(list[i], list[j]);
    }
    if (verbose) std::cout << " [Done]\n";

    for (std::size_t i = 0; i < iter; ++i) unsorted << list[i];
    unsorted.flush();

    Stream<IntType> sorted(dest + ".sorted");
    std::sort(list.begin(), list.end());
    for (auto i : list) sorted << i;
    sorted.flush();
}

int main(int argc, char** argv) {
    argparse::ArgumentParser args("datagen");
    args.add_argument("--N")
        .required();
    
    args.add_argument("--dist")
        .required()
        .choices("uniform", "normal", "bimodal") // TODO: exponential, zipfian, randomwalk, fewunique, constant
        .default_value("uniform");
    
    args.add_argument("--pattern")
        .choices("random", "almost", "noise", "reversed", "sawtooth", "bitonic", "frontsorted", "gap")
        .default_value("random");
    
    /**********************************************************/
    /*      almost : global random swap                       */
    /*       noise : local random swap with sliding window    */
    /*    reversed : reverse sorted                           */
    /*    sawtooth : (merge-like) locally ascending order     */
    /*     bitonic : alternating ascending/descending order   */
    /* frontsorted : the front half of data is already sorted */
    /*         gap : large gap between two consecutive data   */
    /**********************************************************/
    
    args.add_argument("--bsize")
        .scan<'i', std::int16_t>()
        .choices(8, 16, 32, 64)
        .default_value((std::int16_t)32);

    args.add_argument("--seed")
        .scan<'i', std::int64_t>()
        .default_value((std::int64_t)std::random_device()());
    
    // args.add_argument("--almost")
    //     .scan<'i', int>()
    //     .default_value(0);
    
    args.add_argument("--verbose")
        .default_value(false)
        .implicit_value(true);

    args.add_argument("--path")
        .default_value(std::string("./dataset/"));
    
    try { args.parse_args(argc, argv); }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << args;
        std::exit(1);
    }

    const std::string iter_suffix = args.get<std::string>("--N");
    const std::size_t iter = parse_suffix(iter_suffix);
    const std::int64_t seed = args.get<std::int64_t>("--seed");
    const std::int16_t bsize = args.get<std::int16_t>("--bsize");
    const std::string dist = args.get<std::string>("--dist");
    const std::string pattern = args.get<std::string>("--pattern");
    const std::filesystem::path path = std::filesystem::absolute(args.get<std::string>("--path"));
    const bool verbose = args.get<bool>("--verbose");

    std::mt19937 engine(seed);
    std::string dest = std::format("int{}_{}_{}_{}", bsize, iter_suffix, dist, pattern);
    dest = resolve_duplicated(path, dest);

    if (verbose) {
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "======================= SUMMARY =======================\n"
                  << "  Random Seed : " << seed << "\n"
                  << " Distribution : " << dist << "\n"
                  << "   Iterations : " << iter << "\n"
                  << "   Data Range : " << "[0, 2^" << bsize << "-1]\n"
                  << "    Data Size : " << bsize / 8 << " Bytes\n"
                  << "  Target Size : " << iter * bsize / 8388608. << " MiB\n"
                  << "       Target : " << dest << ".*\n"
                  << "=======================================================\n";
    }

    switch (bsize) {
    case  8: generate<std::uint8_t >(iter, dist, pattern, engine, path / dest, verbose); break;
    case 16: generate<std::uint16_t>(iter, dist, pattern, engine, path / dest, verbose); break;
    case 32: generate<std::uint32_t>(iter, dist, pattern, engine, path / dest, verbose); break;
    case 64: generate<std::uint64_t>(iter, dist, pattern, engine, path / dest, verbose); break;
    }

    return 0;
}