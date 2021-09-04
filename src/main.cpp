#include "color.h"
#include "constants.h"
#include "hittable_list.h"
#include "sphere.h"
#include "common.h"
#include "camera.h"
#include "material.h"
#include "utils.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <memory>
#include <cassert>

#include <omp.h>

using std::cout;
using std::cerr;
using std::endl;

void ray_color(const ray& r, const hittable_list& world, int depth, ray_return &ret) {
	if(depth < 0) {
		ret.depth = max_raytrace_depth;
		ret.ray_color = {0, 0, 0};
		return;
	}
	hit_record rec;
    if (world.hit(r, 0.0001, infinity, rec)) {
		ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
			ray_color(scattered, world, depth-1, ret);
			ret.ray_color *= attenuation;
			return;
		} else if(rec.mat_ptr->glows){
			ret.depth = max_raytrace_depth - depth;
			ret.ray_color = attenuation;
			return;
		}
		ret.depth = max_raytrace_depth - depth;
		ret.ray_color = {0, 0, 0};
		return;
    }
	/*
	return color(1.0, 1.0, 1.0);
	*/
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    ret.ray_color = (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
	ret.depth = max_raytrace_depth - depth - 1;
	return;
	/*
	return color(0.0, 0.0, 0.0);
	*/
}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

int main() {
	omp_set_num_threads(2);
	auto out_file = open_output(output_name);
	auto hm_file = open_output(heatmap_name);

	std::cerr << "Testing random: " << random_in_unit_sphere().x() << endl;

	// World
	hittable_list world = random_scene();

	// Camera
	camera cam(
		lookfrom,
		lookat,
		vup,
		vfov,
		aspect_ratio,
		aperture,
		dist_to_focus
	);

	init_stream_picture(out_file);
	init_stream_picture(hm_file);

	int64_t total_num_skipped = 0;
	const int64_t total_num_to_be_done = max_samples_per_pixel * 1ll * image_width * image_height;

	for(int j = image_height-1; j >= 0; j--) {
		cerr << "\rDone:" << std::setprecision(5) << 
			((double(image_height - j) * 100.0) / double(image_height))
			<< "       " << std::flush;

		for(int i = 0; i < image_width; i++) {
			int total_samples = 0;
			int64_t curr_skipped = 0;
			color pixel_color = color(0,0,0);
			#pragma omp parallel
			{
				ray_return return_ray;
				vec3 sm = vec3(0,0,0);
				vec3 var = vec3(infinity, infinity, infinity);
				vec3 sm_sq = vec3(0,0,0);
				vec3 thr = vec3(0,0,0);
				color col = color(0,0,0);
				int sm_ray_changes = 0;
				int current_sample = 0;
				int next_var_check = min_samples_per_pixel;
				bool done = false;
				#pragma omp for reduction (+:total_samples, curr_skipped)
				for (int _s = 0; _s < max_samples_per_pixel; _s++) {
					if(done){
						curr_skipped++;
						continue;
					}
					col.clear();
					for(int grp = 0; grp < group_rays_by; grp++) {
						auto u = (i + random_double()) / (image_width-1);
						auto v = (j + random_double()) / (image_height-1);
						ray r = cam.get_ray(u, v);
						ray_color(r, world, max_raytrace_depth-1, return_ray);
						col += return_ray.ray_color;
						sm_ray_changes += return_ray.depth;
					}
					col /= group_rays_by;
					current_sample++;
					total_samples++;
					sm += col;
					sm_sq += col * col;
					if(current_sample == next_var_check) {
						var = sm_sq - (sm * sm) / current_sample;
						thr = max_diff * max_diff * sm * sm;
						const double avg_ray_changes = (double)(sm_ray_changes) / (double)(current_sample * group_rays_by);
						if(var[0] <= thr[0] && var[1] <= thr[1] && var[2] <= thr[2]) {
							done = true;
						} else if(avg_ray_changes < min_avg_ray_changes){
							done = true;
						} else {
							next_var_check += next_var_check >> 1;
						}
					}
				}
				#pragma omp critical
				{
					pixel_color += sm;
				}
			}
            write_color(out_file, pixel_color, total_samples);
			write_color(hm_file, color(double(curr_skipped) / double(max_samples_per_pixel - min_samples_per_pixel), 0.0, 0.0), 1);
			total_num_skipped += curr_skipped;
		}
	}

	cerr << "\nDone" << endl;
	cerr << "Skipped " << total_num_skipped << " from " << total_num_to_be_done << endl;
	return 0;
}
