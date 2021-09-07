#pragma once

#include <string>
#include <vector>

#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>

#include "hittable.h"
#include "material.h"
#include "vec3.h"
#include "hittable_list.h"
#include "hittable.h"
#include "sphere.h"

namespace scene {
    using json = nlohmann::json;
    struct object_properties {
        vec3 position;
        color albedo;
        double refraction_ratio;
        double reflectivity;
        double fuzziness;
        double scale;
    };

    struct scene_object{
        hittable_type object_type;
        material_type object_material;
        object_properties properties;
    };

    struct render_properties{
        int image_width;
        int image_height;
        int group_rays_by;
        int min_samples_per_pixel;
        int max_samples_per_pixel;
        int max_raytrace_depth;
        int color_bits;

        double max_giveup_difference;
        double min_ray_bounces_giveup_threshold;

        std::string output_name;
        std::string heatmap_name;
    };

    struct camera_properties{
        point3 lookfrom;
        point3 lookat;
        vec3 vup;
        double dist_to_focus;
        double aperture;
        double vfov;
        double aspect_ratio;
    };

    struct scene_files{
        std::map<std::string, std::string> file; // map<filename, file content>
    };

    struct scene_description{
        std::vector<scene_object> objects;
        render_properties render;
        camera_properties camera;
        scene_files files;
    };

    void from_json(const json& j, scene_object& s);
    void from_json(const json& j, object_properties& p);
    void to_json(json& j, const scene_object& s);
    void to_json(json& j, const object_properties& p);

    template<typename T>
    void from_json(const json& j, std::vector<T>& v);
    template<typename T>
    void to_json(json& j, const std::vector<T>& v);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        render_properties,
        image_width,
        image_height,
        group_rays_by,
        min_samples_per_pixel,
        max_samples_per_pixel,
        max_raytrace_depth,
        color_bits,

        max_giveup_difference,
        min_ray_bounces_giveup_threshold,

        output_name,
        heatmap_name
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        camera_properties,
        lookfrom,
        lookat,
        vup,
        dist_to_focus,
        aperture,
        vfov
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        scene_files,
        file
    );

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        scene_description,
        objects,
        render,
        camera
    );

    scene_description read(const std::string& filename);
    hittable_list get_world(const scene_description& desc);
};