//
// Created by kkeiper1103 on 5/30/2023.
//

#ifndef PREDATION_CHARACTERFACTORY_H
#define PREDATION_CHARACTERFACTORY_H

#include "IFactory.h"
#include "models/Character.h"

class CharacterFactory : public Factory<Character> {
public:
    virtual ~CharacterFactory() {
        for(auto& kv: items)
            free_mesh(kv.second);

        items.clear();
    }

    std::shared_ptr<Character> Get(const std::string& filename) override {
        if( items.find(filename) == items.end() ) {
            items[filename] = load_car_file(filename.c_str());
        }

        return std::shared_ptr<Character>(new Character(&items[filename]));
    }

protected:
    std::unordered_map<std::string, Character::Data> items;
};


#endif //PREDATION_CHARACTERFACTORY_H
