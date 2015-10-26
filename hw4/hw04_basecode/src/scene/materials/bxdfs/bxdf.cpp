#include <scene/materials/bxdfs/bxdf.h>


glm::vec3 BxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const
{
    //TODO
    //everything here is in normal local space
    glm::vec3 sampled_color;

    wi_ret.z = rand1;
    wi_ret.x = glm::cos(TWO_PI*rand2)*glm::sqrt(1 - rand1*rand1);
    wi_ret.y = glm::sin(TWO_PI*rand2)*glm::sqrt(1 - rand1*rand1);

    wi_ret = glm::normalize(wi_ret);
    pdf_ret = PDF(wo, wi_ret);

    //divide by the pdf_ret for importance sampling
    sampled_color = EvaluateScatteredEnergy(wo, wi_ret)/pdf_ret;


    return sampled_color;
}

glm::vec3 BxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2* samples) const
{
    //TODO
    //everything here is in normal local space
    glm::vec3 wi_temp(0, 0, 0);
    glm::vec3 final_color(0, 0, 0);
    float rand1(0.0f);
    float rand2(0.0f);
    float pdf_temp(0.0f);

    for (int i = 0; i < num_samples; i++)
    {
        rand1 = (samples + i)->x;
        rand2 = (samples + i)->y;

        final_color = final_color + SampleAndEvaluateScatteredEnergy(wo, wi_temp, rand1, rand2, pdf_temp);
    }
    // don't forget to divide by N
    final_color = final_color/static_cast<float>(num_samples);
    return final_color;
}

float BxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    //TODO
    //everything here is in normal local space
    glm::vec3 default_normal(0, 0, 1);
    //wi should be normalized coming in, if not things go to shit
    float cos_theta(glm::dot(wi, default_normal));
    float pdf_result(1.0f/PI*cos_theta);
    //wo might have some effect on certain materials, but for now let's not worry about it
    return pdf_result;
}
