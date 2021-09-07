#include "scene.h"

#include <fstream>
#include <memory>

void scene::from_json(const json &j, object_properties& p){
    j["position"].get_to(p.position);
    if(j.contains("albedo")){
        j["albedo"].get_to(p.albedo);
    } else {
        p.albedo = {1.0, 1.0, 1.0};
    }
    if(j.contains("refraction_ratio")){
        j["refraction_ratio"].get_to(p.refraction_ratio);
    } else {
        p.refraction_ratio = 1.0;
    }
    if(j.contains("reflectivity")){
        j["reflectivity"].get_to(p.reflectivity);
    } else {
        p.reflectivity = 1.0;
    }
    if(j.contains("fuzziness")){
        j["fuzziness"].get_to(p.fuzziness);
    } else {
        p.fuzziness = 0.0;
    }
    if(j.contains("scale")){
        j["scale"].get_to(p.scale);
    } else {
        p.scale = 1.0;
    }
}

void scene::from_json(const json& j, scene_object& s){
    if(j["object_type"] == "sphere"){
        s.object_type = hittable_type::SPHERE;
    }
    if(j["material_type"] == "metal"){
        s.object_material = material_type::METAL;
    } else if(j["material_type"] == "lambertian"){
        s.object_material = material_type::LAMBERTIAN;
    } else if(j["material_type"] == "dielectric"){
        s.object_material = material_type::DIELECTRIC;
    }
    j.get_to(s.properties);
}
void scene::to_json(json& j, const scene_object& s){
    // TODO: implement me
    throw "Not implemented";
}
void scene::to_json(json& j, const object_properties& p){
    // TODO: implement me
    throw "Not implemented";
}

template<typename T>
void scene::from_json(const json& j, std::vector<T>& v){
    v.clear();
    v.reserve(j.size());
    for(const auto &object:j){
        v.push_back(object.get<T>());
    }
}
template<typename T>
void scene::to_json(json& j, const std::vector<T>& v){
    j.clear();
    for(const auto &object:v){
        j.push_back(object);
    }
}

scene::scene_description scene::read(const std::string &filename){
    std::ifstream input(filename);
    json result;
    input >> result;
    auto ret = result.get<scene::scene_description>();
    ret.camera.aspect_ratio = (double)ret.render.image_width / (double)ret.render.image_height;
    return ret;
}

hittable_list scene::get_world(const scene_description &desc){
    hittable_list world;
    for(const auto& object:desc.objects){
        shared_ptr<material> mat_ptr = nullptr;
        switch (object.object_material) {
            case material_type::LAMBERTIAN:
                mat_ptr = std::make_shared<lambertian>(object.properties.albedo);
                break;
            case material_type::DIELECTRIC:
                mat_ptr = std::make_shared<dielectric>(object.properties.albedo, object.properties.refraction_ratio);
                break;
            case material_type::METAL:
                mat_ptr = std::make_shared<metal>(object.properties.albedo, object.properties.fuzziness);
                break;
            default:
                throw "Invalid material type";
        }
        shared_ptr<hittable> obj_ptr = nullptr;
        switch (object.object_type) {
            case hittable_type::SPHERE:
                obj_ptr = std::make_shared<sphere>(object.properties.position, object.properties.scale, mat_ptr);
                break;
            default:
                throw "Invalid object type";
        }
        world.add(obj_ptr);
    }
    return world;
}