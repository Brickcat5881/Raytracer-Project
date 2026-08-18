#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H


#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "hittable_list.h"
#include "bvh.h"

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
    shared_ptr<material> mat;
    hittable_list triangles;
    shared_ptr<bvh> bvh_tree;

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

                triangles.add(make_shared<triangle>(p1,p2,p3,mat));

            }
        }

        bvh_tree = make_shared<bvh>(triangles);
    }
    
};

#endif