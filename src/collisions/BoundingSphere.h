//
// Created by kkeiper1103 on 1/8/23.
//

#ifndef PREDATION_BOUNDINGSPHERE_H
#define PREDATION_BOUNDINGSPHERE_H


#include "CollisionShape.h"

class BoundingSphere : public CollisionShape {
public:
    bool intersects(CollisionShape *other) override;
};


#endif //PREDATION_BOUNDINGSPHERE_H
