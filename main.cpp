#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "triangle.h"
#include "model_loader.h"
#include "translate.h"


int main() {
    hittable_list world;

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
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.3;
    cam.focus_dist    = (cam.lookfrom - cam.lookat).length();

    cam.render(world);
}