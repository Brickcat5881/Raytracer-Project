#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H


#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include "hittable_list.h"
#include "bvh.h"

//Another hittable object like sphere or triangle that consists of a vector of triangles
class model: public hittable {
    public:
    model(const std::string path, shared_ptr<material> mat) : path(path), mat(mat) {
        load_file(path);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        return bvh_tree->hit(r, ray_t, rec);
    }

    aabb bounding_box() const override {
        return bvh_tree->bounding_box();
    }


    private:
    std::string path;
    std::string mat_path;
    shared_ptr<material> mat;
    hittable_list triangles;
    shared_ptr<bvh> bvh_tree;
    shared_ptr<material> current_mat;
    std::map<std::string, shared_ptr<material>> loaded_materials;

    void load_file(const std::string filePath) {
        std::ifstream file(filePath);
        std::vector<point3> vertices;

        if (!file.is_open()) {
            std::cerr << "Could not open file: " << path << "\n";
            return;
        }

        std::string line;
        while (std::getline(file,line)) {
            if (line.empty()) {
                continue;
            }
            std::stringstream ss(line);
            std::string prefix;
            ss >> prefix; // This reads the first word in the line

            if (prefix == "v") {
                double x, y, z;
                ss >> x >> y >> z; //Takes the second, third and fourth words and assigns them to x,y,z
                vertices.push_back(point3(x, y, z));
            }

            if (prefix == "f") {
                std::string word1, word2, word3;
                ss >> word1 >> word2 >> word3;

                int v1 = std::stoi(word1.substr(0, word1.find('/')));
                int v2 = std::stoi(word2.substr(0, word2.find('/')));
                int v3 = std::stoi(word3.substr(0, word3.find('/')));

                point3 p1 = vertices[v1 - 1];
                point3 p2 = vertices[v2 - 1];
                point3 p3 = vertices[v3 - 1];

                auto assigned_mat = (current_mat != nullptr) ? current_mat : mat;
                triangles.add(make_shared<triangle>(p1, p2, p3, assigned_mat));

            }
            
            if (prefix == "mtllib") {
                
                ss >> mat_path;
                mat_path = "OBJs/" + mat_path;
                load_materials(mat_path);
            }

            if (prefix == "usemtl") {
                std::string mat_name;
                ss >> mat_name;
                current_mat = loaded_materials[mat_name];

            }
        }

        bvh_tree = make_shared<bvh>(triangles);
    }

    void load_materials(const std::string& path) {
        std::string current_name;
        float current_Ns;   //Fuzz
        color current_Kd;   //Albedo / Colour
        color current_Ks;   //Specular
        color current_Ke;   //Emission
        float current_Ni;   //IOR
        float current_d;    //Opacity

        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Could not open file: " << path << "\n";
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) {
                    continue;
                }

            std::stringstream ss(line);
            std::string prefix;
            ss >> prefix;
            if (prefix == "newmtl") {
                if (current_name != "") {
                    if (current_Ke.x() > 0 || current_Ke.y() > 0 || current_Ke.z() > 0) {
                        continue; //ADD EMMISIVE
                    }
                    else if (current_d < 1.0) {
                        loaded_materials[current_name] = make_shared<dielectric>(current_Ni);
                    }
                    else if (current_Ks.x() > 0.2 || current_Ks.y() > 0.2 || current_Ks.z() > 0.2) {
                        double fuzz = 1.0 - (current_Ns / 1000.0);
                        loaded_materials[current_name] = make_shared<metal>(current_Kd,fuzz);
                    }
                    else {
                        loaded_materials[current_name] = make_shared<lambertian>((current_Kd));
                    }
                }
                ss >> current_name;
            } 
            else if (prefix == "Ns") {
                ss >> current_Ns;
            }
            else if (prefix == "Kd") {
                double value1,value2,value3;
                ss >> value1 >> value2 >> value3;
                current_Kd = color(value1,value2,value3);
            }
            else if (prefix == "Ks") {
                double value1,value2,value3;
                ss >> value1 >> value2 >> value3;
                current_Ks = color(value1,value2,value3);
            }
            else if (prefix == "Ke") {
                double value1,value2,value3;
                ss >> value1 >> value2 >> value3;
                current_Ke = color(value1,value2,value3);
            }
            else if (prefix == "Ni") {
                ss >> current_Ni;
            }
            else if (prefix == "d") {
                ss >> current_d;
            }


        }
        if (current_Ke.x() > 0 || current_Ke.y() > 0 || current_Ke.z() > 0) {
            return; //ADD EMMISIVE
        }
        else if (current_d < 1.0) {
            loaded_materials[current_name] = make_shared<dielectric>(current_Ni);
        }
        else if (current_Ks.x() > 0.2 || current_Ks.y() > 0.2 || current_Ks.z() > 0.2) {
            double fuzz = 1.0 - (current_Ns / 1000.0);
            loaded_materials[current_name] = make_shared<metal>(current_Kd,fuzz);
        }
        else {
            loaded_materials[current_name] = make_shared<lambertian>((current_Kd));
        }
    }
};

#endif