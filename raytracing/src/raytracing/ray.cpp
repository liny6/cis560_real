#include <raytracing/ray.h>

Ray::Ray(const glm::vec3 &o, const glm::vec3 &d):
    origin(o),
    direction(glm::normalize(d)),
    transmitted_color(1,1,1)
{}

Ray::Ray(const glm::vec4 &o, const glm::vec4 &d):
    Ray(glm::vec3(o), glm::vec3(d))
{}

Ray::Ray(const Ray &r):
    Ray(r.origin, r.direction)
{
    transmitted_color = r.transmitted_color;
}

Ray Ray::GetTransformedCopy(const glm::mat4 &T) const
{
    //TODO: Implement this correctly!
    glm::vec4 origin_new(origin, 1);
    glm::vec4 direction_new(direction, 0);

    origin_new = T*origin_new;
    direction_new = T*direction_new;

    Ray Transformed(origin_new, direction_new);

    return Transformed;
}
