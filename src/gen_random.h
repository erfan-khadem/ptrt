#pragma once

#include <cstdint>
#include <random>
#include <chrono>

#include <omp.h>

inline double random_double() {
    static std::vector<std::mt19937> generators;
    if(generators.empty()){
        const int64_t tmp_seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::mt19937 tmp_gen = std::mt19937(tmp_seed);
        std::uniform_int_distribution<int64_t> dist;
        for(int i = 0; i < omp_get_max_threads(); i++){
            generators.push_back(std::mt19937(dist(tmp_gen)));
        }
    }
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generators[omp_get_thread_num()]);
}

inline double random_double(const double min_val, const double max_val) {
    auto rnd = random_double();
    return ((max_val - min_val) * rnd) + min_val;
}