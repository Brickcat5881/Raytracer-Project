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


int main() {
    //Opening the JSON file for the scene
    std::ifstream scene_file("Scenes/ringScene.json");
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
            materials[name] = make_shared<metal>(albedo);
        }
    }




    //auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    //world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    auto triangle_material = make_shared<lambertian>(color(0.7, 0.5, 0.5));
    //world.add(make_shared<triangle>(point3(-2,0,0), point3(2,0,0), point3(0,3,0), triangle_material));

    auto torus = make_shared<model>("OBJs/simpleRing.obj", triangle_material);
    world.add(make_shared<translate>(torus,vec3(0,1,0)));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 12;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0); //Controls Roll, Input direction of vertically upwards for scene

    cam.defocus_angle = 0.3; //Essetially aperture
    cam.focus_dist    = (cam.lookfrom - cam.lookat).length(); //Focus Distance, currently set at being the lookat point

    cam.render(world);
}