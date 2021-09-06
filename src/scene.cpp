#include "scene.h"
#include <fstream>
#include <memory>

void scene::from_json(const json &j, object_properties& p){
    if(j.contains("position")){
        j["position"].get_to(p.position);
    }
    if(j.contains("color")){
        j["color"].get_to(p.color);
    }
    j["refraction_ratio"].get_to(p.refraction_ratio);
    j["reflectivity"].get_to(p.reflectivity);
    j["fuzziness"].get_to(p.fuzziness);
    j["scale"].get_to(p.scale);
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
    return result.get<scene::scene_description>();
}

hittable_list scene::get_world(const scene_description &desc){
    hittable_list world;
    for(const auto& object:desc.objects){
        shared_ptr<material> mat_ptr = nullptr;
        switch (object.object_material) {
            case material_type::LAMBERTIAN:
                mat_ptr = std::make_shared<lambertian>(object.properties.color);
                break;
            case material_type::DIELECTRIC:
                mat_ptr = std::make_shared<dielectric>(object.properties.color, object.properties.refraction_ratio);
                break;
            case material_type::METAL:
                mat_ptr = std::make_shared<metal>(object.properties.color, object.properties.fuzziness);
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