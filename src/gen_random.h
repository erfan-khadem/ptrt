#pragma once

#include <random>
#include <omp.h>

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generators[] = {
        std::mt19937(31607),
        std::mt19937(25409),
        std::mt19937(25747),
        std::mt19937(31957),
        std::mt19937(28571),
        std::mt19937(28477),
        std::mt19937(27487),
        std::mt19937(31957)
    };
    return distribution(generators[omp_get_thread_num()]);
}

inline double random_double(const double min_val, const double max_val) {
    auto rnd = random_double();
    return ((max_val - min_val) * rnd) + min_val;
}