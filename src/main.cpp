#include "color.h"
#include "constants.h"
#include "hittable_list.h"
#include "sphere.h"
#include "common.h"
#include "camera.h"
#include "material.h"
#include "utils.h"
#include "scene.h"
#include "vec3.h"

#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <memory>
#include <cassert>

#include <omp.h>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;

void ray_color(
	const ray& r, 
	const hittable_list& world, 
	int depth, 
	const int max_raytrace_depth,
	ray_return &ret
);

enum program_type {
	WORKER,
	STANDALONE
};

struct job{
	program_type type;
	std::string input_filename;
	job(const program_type _tp, const std::string& file){
		type = _tp;
		input_filename = file;
	}
};

struct tile {
	int start_h;
	int end_h;
	int start_v;
	int end_v;

	shared_ptr<std::vector<color>> result;
	shared_ptr<std::vector<color>> heat_map;

	tile(){}
	tile(const int _sh, const int _eh, const int _sv, const int _ev) :
		start_h(_sh), end_h(_eh), start_v(_sv), end_v(_ev) {}

	shared_ptr<std::vector<color>> calculate(
		const camera& cam,
		const hittable_list& world,
		const scene::render_properties& rp
	){
		assert(start_h < end_h);
		assert(start_v < end_v);
		const int64_t tile_size = (end_h - start_h) * (end_v - start_v);
		result = std::make_shared<std::vector<color>>(tile_size);
		heat_map = std::make_shared<std::vector<color>>(tile_size);
		int current_pixel = 0;
		for(int i = start_h; i < end_h; i++){
			for(int j = start_v; j < end_v; j++){
				ray_return return_ray;
				vec3 sm = vec3(0,0,0);
				vec3 var = vec3(infinity, infinity, infinity);
				vec3 sm_sq = vec3(0,0,0);
				vec3 thr = vec3(0,0,0);
				color col = color(0,0,0);
				int sm_ray_changes = 0;
				int num_samples = rp.max_samples_per_pixel;
				int next_var_check = rp.min_samples_per_pixel;
				for (int sample = 0; sample < rp.max_samples_per_pixel; sample++) {
					col.clear();
					for(int grp = 0; grp < rp.group_rays_by; grp++) {
						auto u = (i + random_double()) / (rp.image_width-1);
						auto v = (j + random_double()) / (rp.image_height-1);
						ray r = cam.get_ray(u, v);
						ray_color(r, world, rp.max_raytrace_depth-1, rp.max_raytrace_depth, return_ray);
						col += return_ray.ray_color;
						sm_ray_changes += return_ray.depth;
					}
					col /= rp.group_rays_by;
					sm += col;
					sm_sq += col * col;
					if(sample == next_var_check) {
						var = sm_sq - (sm * sm) / sample;
						thr = rp.max_giveup_difference * rp.max_giveup_difference * sm * sm;
						const double avg_ray_changes = (double)(sm_ray_changes) / (double)(sample * rp.group_rays_by);
						if(var[0] <= thr[0] && var[1] <= thr[1] && var[2] <= thr[2]) {
							num_samples = sample + 1;
							break;
						} else if(avg_ray_changes < rp.min_ray_bounces_giveup_threshold){
							num_samples = sample + 1;
							break;
						} else {
							next_var_check += next_var_check >> 1;
						}
					}
				}
				result->at(current_pixel) = sm / num_samples;
				heat_map->at(current_pixel) = (1.0 - ((num_samples - rp.min_samples_per_pixel) 
						/ (double)(rp.max_samples_per_pixel - rp.min_samples_per_pixel + 1)))
					* color(0, 0.5, 1.0);
				
				current_pixel++;
			}
		}
		return result;
	}
};

void ray_color(
	const ray& r, 
	const hittable_list& world, 
	int depth, 
	const int max_raytrace_depth,
	ray_return &ret
){
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
			ray_color(scattered, world, depth-1, max_raytrace_depth, ret);
			ret.ray_color *= attenuation;
			return;
		}
		ret.depth = max_raytrace_depth - depth;
		ret.ray_color = {0, 0, 0};
		return;
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    ret.ray_color = (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
	ret.depth = max_raytrace_depth - depth - 1;
	return;
}

std::vector<tile> split_to_tiles(const scene::render_properties& rp){
	std::vector<tile> result;
	const int64_t image_pixels = rp.image_width * 1LL * rp.image_height;
	const int64_t tile_pixels = rp.tile_width * 1LL * rp.tile_height;
	result.reserve(std::max((int64_t)1, int64_t(image_pixels / tile_pixels) + 100));
	for(int i = 0; i < rp.image_width; i+=rp.tile_width){
		for(int j = 0; j < rp.image_height; j+=rp.tile_height){
			result.push_back(tile(
							i, std::min(rp.image_width,i+rp.tile_width),
							j, std::min(rp.image_height,j+rp.tile_height)));
		}
	}
	return result;
}

void sort_tiles(std::vector<tile> &tls){
	std::sort(
		tls.begin(),
		tls.end(),
		[=](const tile& t1, const tile& t2){
			return t1.start_h == t2.start_h ? t1.start_v < t2.start_v : t1.start_h < t2.start_h;
		}
	);
}

void print_usage(const std::string& program_name){
	cerr << "Usage: " 
		<< program_name
		<< " (worker | standalone) (worker|scene description)"
		<< endl;
	exit(EXIT_FAILURE);
}

job get_job(const int argc, const char** argv){
	if(argc != 3){
		print_usage(argv[0]);
		//Exits here
	} else {
		if(std::string(argv[1]) == "worker"){
			return job(program_type::WORKER, argv[2]);
		} else if(std::string(argv[1]) == "standalone"){
			return job(program_type::STANDALONE, argv[2]);
		} else {
			print_usage(argv[0]);
			//Exits here
		}
	}
	throw "Invalid state";
}

void tiles_to_pixel_buf(
	const std::vector<tile>& tiles, 
	std::vector<color>& output, 
	std::vector<color>& heat_map,
	const scene::render_properties& rp
){
	for(const auto& tl:tiles){
		int pos = 0;
		for(int i = tl.start_h; i < tl.end_h; i++){
			for(int j = tl.start_v; j < tl.end_v; j++){
				output.at(j * rp.image_width + i) = (*tl.result).at(pos);
				heat_map.at(j * rp.image_width + i) = (*tl.heat_map).at(pos);
				pos++;
			}
		}
	}
}

int main(const int argc, const char** argv) {
	const auto job = get_job(argc, argv);
	cerr << "Testing random: " << random_double() << endl;
	if(job.type == program_type::WORKER){
		throw "worker still not implemented";
	} else {
		assert(job.type == program_type::STANDALONE); //Make sure we are not in an invalid state

		auto scene_desc = scene::read(job.input_filename);

		cerr << "Starting to render `" << scene_desc.render.render_name << "`" << endl;

		// World
		const auto world = scene::get_world(scene_desc);

		// Camera
		camera cam(
			scene_desc.camera.lookfrom,
			scene_desc.camera.lookat,
			scene_desc.camera.vup,
			scene_desc.camera.vfov,
			scene_desc.camera.aspect_ratio,
			scene_desc.camera.aperture,
			scene_desc.camera.dist_to_focus
		);

		auto out_file = open_output(scene_desc.render.output_name);
		auto hm_file  = open_output(scene_desc.render.heatmap_name);

		init_stream_picture(out_file, scene_desc);
		init_stream_picture(hm_file, scene_desc);

		auto tiles = split_to_tiles(scene_desc.render);
		int tasks_done = 0;
		//receiving tiles from a remote server and storing them to send them later
		#pragma omp parallel
		{
			#pragma omp single
			for(int i = 0; i < (int)tiles.size(); i++){
				#pragma omp task shared(world, cam, scene_desc)
				{
					tiles[i].calculate(cam, world, scene_desc.render);
					#pragma omp critical
					{
						tasks_done++;
						cerr << "\rDone " << tasks_done << "/" << tiles.size() << " tiles\t\t\t\t";
					}
				}
			}
		}
		std::vector<color> pixel_buf(
			scene_desc.render.image_width * scene_desc.render.image_height
		);
		std::vector<color> heat_map(
			scene_desc.render.image_width * scene_desc.render.image_height
		);
		sort_tiles(tiles); //probably helps with data locality
		tiles_to_pixel_buf(tiles, pixel_buf, heat_map, scene_desc.render);
		for(int j = scene_desc.render.image_height - 1; j >= 0; j--){
			for(int i = 0; i < scene_desc.render.image_width; i++){
				write_color(out_file, pixel_buf.at(j*scene_desc.render.image_width + i), 1, scene_desc);
				write_color(hm_file, heat_map.at(j*scene_desc.render.image_width + i), 1, scene_desc);
			}
		}
		return 0;
	}
}
