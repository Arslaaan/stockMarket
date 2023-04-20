#pragma once

#include <chrono>
#include <iostream>

namespace TimeUtils {
using clock_t = std::chrono::system_clock;
/// @brief Cтрока содержащая chrono::duration приводится к time_point
static clock_t::time_point durationAsTimePoint(const std::string& s) {
    std::stringstream is(s);
    clock_t::rep count;
    is >> count;
    clock_t::time_point result{clock_t::duration{count}};
    return result;
}

/// @brief Timepoint преобразовывается в формат для печати
static std::string getTimeAsStr(std::chrono::system_clock::time_point& t) {
    std::time_t time = std::chrono::system_clock::to_time_t(t);
    std::string time_str = std::ctime(&time);
    time_str.pop_back();
    return time_str;
}
}  // namespace TimeUtils
