//
// Created by kkeiper1103 on 5/27/2023.
//

#include "MeshFactory.h"

MeshFactory::Ptr MeshFactory::Get(const std::string &filename) {
    if(items.find(filename) == items.end()) {
        auto data = load_car_file(filename.c_str());

        // calculate vertices and texCoords from data
        auto mesh = new Mesh();
        items[filename] = MeshFactory::Ptr(mesh);

        free_mesh(data);
    }

    return items[filename];
}
