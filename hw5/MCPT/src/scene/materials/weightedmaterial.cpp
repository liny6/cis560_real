#include <scene/materials/weightedmaterial.h>

WeightedMaterial::WeightedMaterial() : Material()
{
}
WeightedMaterial::WeightedMaterial(const glm::vec3 &color) : Material(color){}

glm::vec3 WeightedMaterial::EvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, float pdf_ret, BxDFType flags) const
{
    //TODO
    //generate some random number
    int res = 100000000;
    float rand1 = static_cast<float>(rand()%res) / static_cast<float>(res);
    float CPF = 0.0f;
    int i = 0;
    int bxdf_choice = 0;

    while(rand1 > CPF)
    {
        CPF = CPF + bxdf_weights[i];
        bxdf_choice = i;
        i++;
    }

    glm::vec3 energy;
    glm::vec3 color;
    //use tangent and bitangent at the intersection to transform the light rays into local frame for brdf
    //find the rotation matrix from local tangents
    glm::mat3 R_inv(isx.tangent, isx.bitangent, isx.normal);
    glm::mat3 R (glm::transpose(R_inv));
    glm::vec3 wo_local(glm::normalize(R*woW));
    glm::vec3 wi_local(glm::normalize(R*wiW));

    color.r = energy.r*base_color.r*isx.texture_color.r;
    color.g = energy.g*base_color.g*isx.texture_color.g;
    color.b = energy.b*base_color.b*isx.texture_color.b;
    pdf_ret = bxdfs[bxdf_choice]->PDF(wo_local, wi_local);


    return color;
}

glm::vec3 WeightedMaterial::SampleAndEvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, glm::vec3 &wiW_ret, float &pdf_ret, float rand1, float rand2, BxDFType flags) const
{
    //TODO
    return glm::vec3();
}
