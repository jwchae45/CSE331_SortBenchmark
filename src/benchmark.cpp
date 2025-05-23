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
        .choices("merge", "heap", "bubble", "insertion", "selection", "quick", "quick_mid", "library", "tim", "cocktail", "comb", "tournament", "introsort");
    
    args.add_argument("--iteration")
        .scan<'i', std::int16_t>()
        .default_value((std::int16_t)10);
    
    args.add_argument("--verbose")
        .default_value(false)
        .implicit_value(true);

    args.add_argument("--dataset")
        .required();
    
    args.add_argument("--result")
        .default_value("./benchmark_result.csv");
    
    try { args.parse_args(argc, argv); }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << args;
        std::exit(1);
    }

    std::ofstream result_csv(args.get<std::string>("--result"), std::ios::app);
    const std::string method = args.get<std::string>("--method");
    const std::string dataset = args.get<std::string>("--dataset");
    const std::int16_t iter = args.get<std::int16_t>("--iteration");
    const bool verbose = args.get<bool>("--verbose");

    if (!result_csv) throw std::runtime_error("Cannot open the file: " + args.get<std::string>("--result"));

    if (verbose) std::cout << std::fixed << std::setprecision(3);

    if (verbose) {
        std::cout << "================= BENCHMARK INFO =================\n"
                  << "      Test Data : " << std::filesystem::path(dataset).filename().string() << "\n"
                  << " Sorting Method : " << method  << "\n"
                  << "      Iteration : " << iter << "\n"
                  << "==================================================\n";
    }

    Mount mnt(dataset + ".unsorted");
    std::unique_ptr<SortBase> sort = [&]() -> std::unique_ptr<SortBase> {
        if (method == "bubble")     return std::make_unique<Bubble    >(mnt);
        if (method == "selection")  return std::make_unique<Selection >(mnt);
        if (method == "insertion")  return std::make_unique<Insertion >(mnt);
        if (method == "merge")      return std::make_unique<Merge     >(mnt);
        if (method == "heap")       return std::make_unique<Heap      >(mnt);
        if (method == "quick")      return std::make_unique<Quick     >(mnt);
        if (method == "quick_mid")  return std::make_unique<QuickMid  >(mnt);
        if (method == "library")    return std::make_unique<Library   >(mnt);
        if (method == "tim")        return std::make_unique<Tim       >(mnt);
        if (method == "cocktail")   return std::make_unique<Cocktail  >(mnt);
        if (method == "comb")       return std::make_unique<Comb      >(mnt);
        if (method == "tournament") return std::make_unique<Tournament>(mnt);
        if (method == "introsort")  return std::make_unique<Introsort>(mnt);
        throw std::runtime_error("Unsupported sorting metod: " + method);
    }();

    TimeLapse<std::ratio<1>> lapse([](double dur) {
        return std::format("[{:>9.3f}] ", dur);
    });
    if (verbose) lapse.start();
    if (verbose) std::cout << lapse() << "Started\n";

    std::vector<BenchResult<ClockResolution>> result;

    int w_iter = check_width(iter);

    for (std::int64_t i = 0; i < iter; ++i) {
        if (verbose) std::cout << lapse() << "Iteration " << std::setw(w_iter) << i+1 << " / " << iter << std::flush;
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
        std::cout << "================ BENCHMARK RESULT ================\n"
                  << "     Input Size (N) : " << mnt.meta.size << "\n"
                  << " Total Elapsed Time : " << total_duration << " ms\n"
                  << "  Mean Elapsed Time : " << std::setw(w_dur) << mean_duration << " ms\n"
                  << std::setprecision(0)
                  << "   # Array Accesses : " << std::setw(m_dur) << mean_access << ". / iteration\n"
                  << "      # Comparisons : " << std::setw(m_dur) << mean_comp << ". / iteration\n"
                  << "==================================================\n";
    }

    // timestamp,method,N,int_size,distribution,pattern,iteration,mean_elapsed,#(array accesses),#(comparisons)
    csv_write_row(result_csv, timestamp(),
                              method,
                              mnt.meta.size,
                              mnt.meta.bsize,
                              mnt.meta.dist,
                              mnt.meta.pattern,
                              iter,
                              std::format("{:.3f}", mean_duration),
                              std::format("{:.0f}.", mean_access),
                              std::format("{:.0f}.", mean_comp));
    return 0;
}