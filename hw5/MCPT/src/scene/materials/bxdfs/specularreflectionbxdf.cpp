#include <scene/materials/bxdfs/specularreflectionbxdf.h>

glm::vec3 SpecularReflectionBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    //TODO
    //define normal vector
    glm::vec3 N(0.0f, 0.0f, 1.0f);
    float residule = glm::length((wo+wi)/2.0f - N);
    //calculate constant k to weigh the reflected energy correctly

    if(fequal(residule, 0.0f))
    {
        float cos_theta = glm::dot(wi, N);
        float sin_theta = glm::length(glm::cross(wi, N));
        float k = 1/cos_theta/sin_theta;
        return k*reflection_color;
    }
    else
    {
        return glm::vec3(0);
    }
}
glm::vec3 SpecularReflectionBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    //TODO
    return glm::vec3(0);
}
