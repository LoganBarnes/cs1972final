#ifndef ROPE_H
#define ROPE_H

#include "verlet.h"

class Rope: public Verlet
{
public:
    //@param units: how many points in the rope
    //@param start: where first point of rope is created
    //@param interval: spacing from one point to the next
    Rope(int units, const glm::vec3& start, const glm::vec3& interval, VerletManager* vm);
    virtual ~Rope();
};

#endif // ROPE_H
