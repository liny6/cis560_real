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
    glm::vec3 H(glm::normalize(wi+wo));
    //define local normal
    glm::vec3 N(0.0f, 0.0f, 1.0f);
    //find G
    float G1 = 1.0f;
    float G2 = 2.0f * glm::dot(N, H) * glm::dot(N, wo) / glm::dot(wo, H);
    float G3 = 2.0f * glm::dot(N, H) * glm::dot(N, wi) / glm::dot(wi, H);
    float G = glm::min(G3, glm::min(G1, G2));
    //use blinn phong distribution
    float D = PDF(wo, wi);
    //calculate fresnel term
    float F = Fresnel(wo, wi);


    glm::vec3 result(reflection_color*D*G*F/4.0f/glm::dot(N, wi)/glm::dot(N, wo));
    return result;
}
glm::vec3 BlinnMicrofacetBxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const
{
    //only sample the brdf lobe
    glm::vec3 N(0.0f, 0.0f, 1.0f);
    glm::vec3 energy(0);
    glm::vec3 lobe_vec(0);
    glm::vec3 wi_lobespace;

    //this is samping the lobe in lobe space
    float cos_theta = glm::pow(rand1, 1/(exponent+1.0f));
    float sin_theta = glm::sqrt(1.0f - cos_theta*cos_theta);
    float phi = 2.0f*PI*rand2;
    //lobe is centered around the reflected ray
    lobe_vec.x = -wo.x;
    lobe_vec.y = -wo.y;
    lobe_vec.z = wo.z;
    //the incident ray need to be converted back into tangent space
    wi_lobespace.x = glm::cos(phi)*sin_theta;
    wi_lobespace.y = glm::sin(phi)*sin_theta;
    wi_lobespace.z = cos_theta;
    //since i have only 1 vector, i cant get a full rotation matrix, but the rotation about the lobe vector does not matter
    //rotate by N cross H by psi
    float cos_psi = glm::dot(lobe_vec, N);
    glm::vec3 sin_psi_skew(glm::cross(N, lobe_vec));
    glm::vec3 u (glm::normalize(sin_psi_skew));
    glm::mat3 outer_u = glm::outerProduct(u, u);
    //use axis angle
    wi_ret = cos_psi*wi_lobespace + sin_psi_skew*wi_lobespace + outer_u*wi_lobespace;
    wi_ret = glm::normalize(wi_ret);

    pdf_ret = PDF(wo, wi_ret);

    energy = EvaluateScatteredEnergy(wo, wi_ret);

    return energy;
}

glm::vec3 BlinnMicrofacetBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    //TODO
    return glm::vec3(0);
}

float BlinnMicrofacetBxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    glm::vec3 H(glm::normalize(wi+wo));
    glm::vec3 N(0.0f, 0.0f, 1.0f);

    float D = (exponent+2.0f)/2.0f/PI * glm::pow(glm::dot(N, H), exponent);
    return D;
}
