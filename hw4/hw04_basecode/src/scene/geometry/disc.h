#pragma once
#include <scene/geometry/geometry.h>

//A disc is aligned with the XY plane with a normal aligned with the positive Z axis. Its radius is 0.5, and it is centered at the origin.
//These attributes can be altered by applying a transformation matrix to the disc.
class Disc : public Geometry
{
public:
    Intersection GetIntersection(Ray r);
    virtual glm::vec2 GetUVCoordinates(const glm::vec3 &point);
    virtual glm::vec3 ComputeNormal(const glm::vec3 &P);
    virtual Intersection GetRandISX(float rand1, float rand2);
    void create();

    virtual void ComputeArea();
};
