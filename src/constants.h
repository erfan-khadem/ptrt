#pragma once

#include <limits>
#include <string>

#include "vec3.h"

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

//ray getter settings
const double max_diff = 0.07;
const double min_avg_ray_changes = 1.05;

//camera settings
const point3 lookfrom = point3(13, 2, 3);
const point3 lookat = point3(0, 0, 0);
const vec3 vup = vec3(0, 1, 0);
const double dist_to_focus = 10.0;
const double aperture = 0.1;
const double vfov = 20;
//const double aspect_ratio = 16.0 / 9.0;
const double aspect_ratio = 3.0 / 2.0;

const int color_bits = 12;
const int image_width = 2400;
const int image_height = static_cast<int>(image_width / aspect_ratio);
const int max_samples_per_pixel = 159;
const int min_samples_per_pixel = 20;
const int group_rays_by = 6;
const int max_raytrace_depth = 50;

const std::string output_name = "out.ppm";
const std::string heatmap_name = "hm.ppm";