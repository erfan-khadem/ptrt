#pragma once

#include "common.h"
#include "scene.h"

#include <iostream>

void write_color(
		std::ostream& out,
		color pixel_color,
		const int samples_per_pixel,
		const scene::scene_description& sd
);
void init_stream_picture(
		std::ostream& out,
		const scene::scene_description& sd
);
