#include <iostream>
#include <cstdint>

#include "argparse.hpp"
#include "filesys.hpp"
#include "sort.hpp"
#include "benchmark.hpp"
#include "verbose.hpp"

using ClockResolution = std::milli;

template<class T>
int check_width(const T& _data, int _precision = -1) {
    std::stringstream ss;
    if (_precision >= 0) ss << std::fixed << std::setprecision(_precision);
    ss << _data;
    return ss.str().size();
}

int main(int argc, char** argv) {
    argparse::ArgumentParser args("benchmark");
    args.add_argument("--method")
        .required()
        .choices("merge", "heap", "bubble", "insertion", "selection", "quick", "library", "tim", "cocktail", "shaker", "comb", "tournament", "introsort");
    
    args.add_argument("--iteration")
        .scan<'i', std::int16_t>()
        .default_value((std::int16_t)10);
    
    args.add_argument("--verbose")
        .default_value(false)
        .implicit_value(true);

    args.add_argument("--dataset")
        .required();
    
    try { args.parse_args(argc, argv); }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << args;
        std::exit(1);
    }

    const std::string method = args.get<std::string>("--method");
    const std::string dataset = args.get<std::string>("--dataset");
    const std::int16_t iter = args.get<std::int16_t>("--iteration");
    const bool verbose = args.get<bool>("--verbose");

    if (verbose) std::cout << std::fixed << std::setprecision(3);

    if (verbose) {
        std::cout << "================ BENCHMARK INFO ================\n"
                  << "        Dataset : " << std::filesystem::path(dataset).filename().string() << "\n"
                  << " Sorting Method : " << method  << "\n"
                  << "      Iteration : " << iter << "\n"
                  << "================================================\n";
    }

    Mount mnt(dataset + ".unsorted");
    std::unique_ptr<SortBase> sort = [&]() {
        if (method == "bubble") return std::make_unique<Bubble>(mnt);
        throw std::runtime_error("Unsupported sorting metod: " + method);
    }();

    TimeLapse<std::ratio<1>> lapse([](double dur) {
        return std::format("[{:>9.4f}] ", dur);
    });
    if (verbose) lapse.start();
    if (verbose) std::cout << lapse() << "Started\n";

    std::vector<BenchResult<ClockResolution>> result;

    int w_iter = check_width(iter);

    for (std::int64_t i = 0; i < iter; ++i) {
        if (verbose) std::cout << lapse() << "Iteration " << std::setw(w_iter) << i+1 << " / " << iter;
        auto bres = benchmark<ClockResolution>(sort);
        if (verbose) std::cout << " => " << bres.duration.count() << " ms\n";
        result.push_back(bres);
        mnt.reset();
    }
    if (verbose) std::cout << lapse() << "Finished\n";
    
    double total_duration = 0., mean_duration = 0.;
    double mean_access = 0., mean_comp = 0;
    for (auto bres : result) {
        total_duration += bres.duration.count();
        mean_access    += double(bres.trace.count_access()) / iter;
        mean_comp      += double(bres.trace.count_comp  ()) / iter;
    }
    mean_duration = total_duration / iter;

    if (verbose) {
        int w_dur = check_width(total_duration, 3);
        int m_dur = check_width(std::max({mean_access, mean_comp}), 0);
        std::cout << "=============== BENCHMARK RESULT ===============\n"
                  << "     Input Size (N) : " << mnt.meta.size << "\n"
                  << " Total Elapsed Time : " << total_duration << " ms\n"
                  << "  Mean Elapsed Time : " << std::setw(w_dur) << mean_duration << " ms\n"
                  << std::setprecision(0)
                  << "   # Array Accesses : " << std::setw(m_dur) << mean_access << ". / iteration\n"
                  << "      # Comparisons : " << std::setw(m_dur) << mean_comp << ". / iteration\n"
                  << "================================================\n";
    }
    return 0;
}