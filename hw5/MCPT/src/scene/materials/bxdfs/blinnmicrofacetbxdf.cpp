#include <scene/materials/bxdfs/blinnmicrofacetbxdf.h>

float Fresnel(glm::vec3 wo, glm::vec3 wi)
{
    //calculates the fresnel term
    glm::vec3 N(0.0f, 0.0f, 1.0f);
    float cos_i = glm::dot(wi, N);
    float cos_o = glm::dot(wo, N);
    //index of refraction for air(incoming)
    float ni = 1.0f;
    //index of refraction for reflective material(just fudge some large number)
    float nt = 10000000.0f;
    //
    float rl = (nt*cos_i - ni*cos_o) / (nt*cos_i + ni*cos_o);
    float rp = (ni*cos_i - nt*cos_o) / (ni*cos_i + nt*cos_o);

    float F = (rl*rl + rp*rp)/2.0f;
    return F;
}

glm::vec3 BlinnMicrofacetBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    //TODO
    //find half vector
    glm::vec3 H((wi+wo)/2.0f);
    //define local normal
    glm::vec3 N(0.0f, 0.0f, 1.0f);
    //find G
    float G1 = 1.0f;
    float G2 = 2.0f * glm::dot(N, H) * glm::dot(N, wo) / glm::dot(wo, H);
    float G3 = 2.0f * glm::dot(N, H) * glm::dot(N, wi) / glm::dot(wo, H);
    float G = glm::min(G3, glm::min(G1, G2));
    //use blinn phong distribution
    float D = PDF(wo, wi);
    //calculate fresnel term
    float F = Fresnel(wo, wi);


    glm::vec3 result(reflection_color*D*G*F/4.0f/glm::dot(N, wi)/glm::dot(N, wo));
    return result;
}
glm::vec3 BlinnMicrofacetBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    //TODO
    return glm::vec3(0);
}

float BlinnMicrofacetBxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    glm::vec3 H((wi+wo)/2.0f);
    glm::vec3 N(0.0f, 0.0f, 1.0f);

    float D = (exponent+2.0f)/2.0f/PI * glm::pow(glm::dot(N, H), exponent);
    return D;
}
