#pragma once

#include "common.h"

#include <iostream>

void write_color(std::ostream& out, color pixel_color, const int samples_per_pixel);
void init_stream_picture(std::ostream& out);