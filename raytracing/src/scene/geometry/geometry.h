#pragma once

#include <raytracing/intersection.h>
#include <openGL/drawable.h>
#include <raytracing/ray.h>
#include <scene/materials/material.h>
#include <scene/transform.h>
#include <scene/geometry/boundingbox.h>

class Intersection;//Forward declaration because Intersection and Geometry cross-include each other
class Material;
class BoundingBox;
class BVHNode;

//Geometry is an abstract class since it contains a pure virtual function (i.e. a virtual function that is set to 0)
class Geometry : public Drawable
{
public:
    Geometry():
    name("GEOMETRY"), transform()
    {
        material = NULL;
        BBX = NULL;
    }

    virtual ~Geometry(){}
    virtual Intersection GetIntersection(Ray r) = 0;
    virtual void SetMaterial(Material* m){material = m;}
    virtual glm::vec2 GetUVCoordinates(const glm::vec3 &point) = 0;
    virtual glm::vec3 RandPTGen() = 0;
    virtual void SetBBX() = 0;
    virtual BVHNode* GetRoot(){return NULL;};
    QString name;//Mainly used for debugging purposes
    Transform transform;
    Material* material;
    BoundingBox* BBX;
};
