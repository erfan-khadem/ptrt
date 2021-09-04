#pragma once

#include "constants.h"

#include <ostream>

double degrees_to_radians(double degrees);
double clamp(double x, double min, double max);
std::ofstream open_output(const std::string filename);