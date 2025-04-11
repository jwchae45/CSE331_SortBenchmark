#ifndef VERBOSE_HPP
#define VERBOSE_HPP

#include <ctime>
#include <chrono>
#include <ratio>
#include <iomanip>
#include <string>
#include <format>
#include <functional>

template<class ClockResolution>
class TimeLapse {
public:
    using Formatter = std::function<std::string(double)>;

    TimeLapse() : fmt([](double dur) -> std::string { return std::format("{:9.3f}", dur); }) {}
    TimeLapse(Formatter _fmt) : fmt(std::move(_fmt)) {}
    
    inline void start()
    { begin = now(); }

    inline std::string operator()() const
    { return fmt(std::chrono::duration<double, ClockResolution>(now() - begin).count()); }

private:
    inline static auto now()
    { return std::chrono::high_resolution_clock::now(); }

private:
    const Formatter fmt;
    std::chrono::high_resolution_clock::time_point begin;
};

class ProgressBar {
};

auto timestamp(void) {
    auto now = std::chrono::system_clock::now();
    std::time_t t_now = std::chrono::system_clock::to_time_t(now);
    return std::put_time(std::localtime(&t_now), "%Y-%m-%d %H:%M:%S");
}

#endif