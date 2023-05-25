//
// Created by kkeiper1103 on 1/8/23.
//

#ifndef PREDATION_COLLISIONSHAPE_H
#define PREDATION_COLLISIONSHAPE_H

class CollisionShape {
public:
    virtual bool intersects(CollisionShape* other) = 0;
};

#endif //PREDATION_COLLISIONSHAPE_H
