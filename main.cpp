#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "triangle.h"
#include "model_loader.h"
#include "translate.h"

#include <map>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

//Essentially a script to initialise all camera values and the contents of the scene from the JSON scene
int main() {
    //Opening the JSON file for the scene
    std::ifstream scene_file("Scenes/CornellBoxScene.json");
    if (!scene_file.is_open()) {
        std::cerr << "Failed to open the scene";
        return 1;
    }

    //Parsing the JSON and turning it into the data variable
    json data = json::parse(scene_file);
    
    hittable_list world;

    //Defining the dictionary "map" as std<KEY_TYPE, VALUE_TYPE>
    //Essentially just defining the types from the json
    std::map<std::string, shared_ptr<material>> materials; 

    for (auto& [name, mat_data] : data["materials"].items()) {
        std::string type = mat_data["material"];

        if (type == "metal") {
            color albedo(mat_data["albedo"][0], mat_data["albedo"][1], mat_data["albedo"][2]);
            auto fuzz(mat_data["fuzz"]);
            materials[name] = make_shared<metal>(albedo,fuzz);
            
            
        }
        else if (type == "lambertian") {
            color albedo(mat_data["albedo"][0], mat_data["albedo"][1], mat_data["albedo"][2]);
            materials[name] = make_shared<lambertian>(albedo);
        }
        else if (type == "dielectric") {
            auto refraction_index(mat_data["refraction_index"]);
            materials[name] = make_shared<dielectric>(refraction_index);
        }
    }

    for (auto& [name, obj_data] : data["objects"].items()) {
        std::string type = obj_data["type"];
        

        if (type == "sphere") {
            std::string mat_name = obj_data["material"];
            point3 center(obj_data["center"][0], obj_data["center"][1], obj_data["center"][2]);
            double radius = obj_data["radius"];
            world.add(make_shared<sphere>(center, radius, materials[mat_name]));
}
        else if (type == "triangle") {
            std::string mat_name = obj_data["material"];
            point3 a(obj_data["a"][0], obj_data["a"][1], obj_data["a"][2]);
            point3 b(obj_data["b"][0], obj_data["b"][1], obj_data["b"][2]);
            point3 c(obj_data["c"][0], obj_data["c"][1], obj_data["c"][2]);
            
            world.add(make_shared<triangle>(a, b, c, materials[mat_name]));
        }
        else if (type == "model") {
            // 1. If JSON specifies a material, grab it; otherwise pass nullptr:
            shared_ptr<material> model_mat = nullptr;
            if (obj_data.contains("material")) {
                model_mat = materials[obj_data["material"]];
            }
            // 2. Create the model:
            auto obj_model = make_shared<model>(obj_data["path"], model_mat);
            if (obj_data.contains("translate")) {
                vec3 offset(obj_data["translate"][0], obj_data["translate"][1], obj_data["translate"][2]);
                world.add(make_shared<translate>(obj_model, offset));
            } else {
                world.add(obj_model);
            }
        }
    }


    camera cam;

    cam.aspect_ratio      = data["camera"]["aspect_ratio"];
    cam.image_width       = data["camera"]["image_width"];
    cam.samples_per_pixel = data["camera"]["samples_per_pixel"];
    cam.max_depth         = data["camera"]["max_depth"];

    cam.vfov     = data["camera"]["vfov"];
    cam.lookfrom = point3(data["camera"]["lookfrom"][0], data["camera"]["lookfrom"][1], data["camera"]["lookfrom"][2]);
    cam.lookat   = point3(data["camera"]["lookat"][0], data["camera"]["lookat"][1], data["camera"]["lookat"][2]);
    cam.vup      = vec3(data["camera"]["vup"][0], data["camera"]["vup"][1], data["camera"]["vup"][2]); //Controls Roll, Input direction of vertically upwards for scene

    cam.defocus_angle = data["camera"]["defocus_angle"]; //Essetially aperture
    if (data["camera"]["focus_dist"] == -1) {
        cam.focus_dist    = (cam.lookfrom - cam.lookat).length(); //Focus Distance, currently set at being the lookat point
    }
    else {
        cam.focus_dist = data["camera"]["focus_dist"];
    }
    
    //RENDER
    cam.render(world);
}