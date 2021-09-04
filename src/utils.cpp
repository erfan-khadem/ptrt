#include "utils.h"
#include "constants.h"
#include <fstream>
#include <iostream>

double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

std::ofstream open_output(const std::string filename) {
    std::ofstream file = std::ofstream(filename);
    if(file.is_open() == false) {
        std::cerr << "Could not open " << "`" << filename << "`" << std::endl;
        exit(1);
    }
    return file;
}