#include <iostream>
#include <iomanip>
#include <format>
#include <cctype>
#include <cstdint>
#include <type_traits>
#include <random>
#include <exception>

#include "argparse.hpp"
#include "filesys.hpp"

template<class IntType>
void generate(const std::int64_t& iter,
              const std::string& dist,
              const std::string& order,
              std::mt19937& engine,
              const std::string& dest,
              bool verbose=false) {
    std::function<IntType()> generator;

    if (dist == "uniform") {
        if constexpr (std::is_same_v<IntType, std::uint64_t>) {
            std::uniform_int_distribution<uint32_t> uniform_dist(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());
            generator = [&]() -> std::uint64_t { return (static_cast<uint64_t>(engine()) << 32) + engine(); };
        } else {
            std::uniform_int_distribution<IntType> uniform_dist(std::numeric_limits<IntType>::min(), std::numeric_limits<IntType>::max());
            generator = [&]() -> IntType { return uniform_dist(engine); };
        }
    } else if (dist == "normal") { // FIXATION NEEDED
        double mu = (std::numeric_limits<IntType>::max() + std::numeric_limits<IntType>::min()) / 2.;
        double sd = (std::numeric_limits<IntType>::max() / (2. * 4.)) - (std::numeric_limits<IntType>::min() / (2. * 4.)); // 4sigma ~ 99.994%
        std::cout << "("  << mu << ", " << sd << ")\n";
        std::normal_distribution<double> normal_dist(mu, sd);
        generator = [&]() { std::cout << normal_dist(engine) << "\n"; return static_cast<IntType>(std::llround(normal_dist(engine))); }; // loose cast?
    } else { throw std::runtime_error("Unsupported distribution: " + dist); }
    
    if (order == "random") {}

    if (verbose) std::cout << "Generating..."; // TODO: add progress bar
    Stream<IntType> unsorted(dest + ".unsorted");
    std::vector<IntType> list(iter);
    IntType g;
    for (std::int64_t i = 0; i < iter; ++i) {
        g = generator();
        list[i] = g;
        unsorted << g;
    }
    unsorted.flush();

    Stream<IntType> sorted(dest + ".sorted");
    std::sort(list.begin(), list.end());
    for (auto i : list) sorted << i;
    sorted.flush();
    if (verbose) std::cout << " [Done]\n";
}

int main(int argc, char** argv) {
    argparse::ArgumentParser args("datagen");
    args.add_argument("--N")
        .required();
    
    args.add_argument("--dist")
        .required()
        .choices("uniform", "normal") // TODO: exponential, zipfian, randomwalk, fewunique
        .default_value("uniform");
    
    args.add_argument("--order")
        .choices("random", "almost", "reversed", "partial")
        .default_value("random");
    
    // almost_sorted => (1) sliding window + random swap
    //                  (2) global random swap (#swap ~ editing distance?)
    //                  (3) locally sorted
    //                  (4) sorted with perturbation(noise)
    //                  (5) ...
    // low entropy?
    
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
    const std::int64_t iter = parse_suffix(iter_suffix);
    const std::int64_t seed = args.get<std::int64_t>("--seed");
    const std::int16_t bsize = args.get<std::int16_t>("--bsize");
    const std::string dist = args.get<std::string>("--dist");
    const std::string order = args.get<std::string>("--order");
    const std::filesystem::path path = std::filesystem::absolute(args.get<std::string>("--path"));
    const bool verbose = args.get<bool>("--verbose");

    std::mt19937 engine(seed);
    std::string dest = std::format("int{}_{}_{}_{}", bsize, iter_suffix, dist, order);
    dest = resolve_duplicated(path, dest);

    if (verbose) {
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "================ SUMMARY ================\n"
                  << "  Random Seed : " << seed << "\n"
                  << " Distribution : " << dist << "\n"
                  << "   Iterations : " << iter << "\n"
                  << "   Data Range : " << "[0, 2^" << bsize << "-1]\n"
                  << "    Data Size : " << bsize / 8 << " Bytes\n"
                  << "  Target Size : " << iter * bsize / 8388608. << " MiB\n"
                  << "       Target : " << dest << ".*\n"
                  << "=========================================\n";
    }

    switch (bsize) {
    case  8: generate<std::uint8_t >(iter, dist, order, engine, path / dest, verbose); break;
    case 16: generate<std::uint16_t>(iter, dist, order, engine, path / dest, verbose); break;
    case 32: generate<std::uint32_t>(iter, dist, order, engine, path / dest, verbose); break;
    case 64: generate<std::uint64_t>(iter, dist, order, engine, path / dest, verbose); break;
    }

    return 0;
}