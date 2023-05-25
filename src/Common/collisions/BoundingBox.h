//
// Created by kkeiper1103 on 1/8/23.
//

#ifndef PREDATION_BOUNDINGBOX_H
#define PREDATION_BOUNDINGBOX_H


#include "CollisionShape.h"

class BoundingBox : public CollisionShape {
public:
    bool intersects(CollisionShape *other) override;
};


#endif //PREDATION_BOUNDINGBOX_H
