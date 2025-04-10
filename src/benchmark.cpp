#include <iostream>

#include "argparse.hpp"
#include "benchmark.hpp"
#include "sort.hpp"

int main(int argc, char** argv) {
    argparse::ArgumentParser args("benchmark");
    args.add_argument("--method")
        .required()
        .choices("merge", "heap", "bubble", "insertion", "selection", "quick", "library", "tim", "cocktail", "shaker", "comb", "tournament", "introsort");
    
    args.add_argument("--iteration")
        .scan<'i', int>()
        .default_value(10);
    
    args.add_argument("--verbose")
        .default_value(false)
        .implicit_value(true);

    args.add_argument("--dataset")
        .default_value(std::string("./dataset/*"));
    
    try { args.parse_args(argc, argv); }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << args;
        std::exit(1);
    }

    std::string method = args.get<std::string>("--method");
    std::string dataset = args.get<std::string>("--dataset");
    int iteration = args.get<int>("--iteration");
    bool verbose = args.get<bool>("--verbose");

    std::cout << method << "\n"
        << dataset << "\n"
        << iteration << "\n"
        << verbose << "\n";

//    auto dataset = mount("dataset/uniform_10k.txt")
//    std::cout << benchmark<Bubble>(dataset);
    return 0;
}