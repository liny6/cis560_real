#include <raytracing/integrator.h>


Integrator::Integrator():
    max_depth(5),
    mersenne_generator(84738384),
    unif_distribution(0.0f, 1.0f)
{
    scene = NULL;
    intersection_engine = NULL;
}

glm::vec3 ComponentMult(const glm::vec3 &a, const glm::vec3 &b)
{
    return glm::vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

//Basic ray trace
glm::vec3 Integrator::TraceRay(Ray r, unsigned int depth)
{
    //TODO
    /*Terminate if too deep*/
    if(depth >= max_depth) return glm::vec3(0, 0, 0);
    /*---------------------*/



    /*Variables We may need*/

    //for the first intersection with the scene and results
    Intersection isx; // first intersection hit,
    glm::vec3 Emitted_Light(0,0,0); // stores the color value of emitted light from the point hit
    glm::vec3 Direct_Lighting(0,0,0); //stores the direct lighting
    float epsilon(0.001); //small distance;
    glm::vec3 Reflected_Light(0,0,0);// stores the color value of reflected light from the point hit
    unsigned int num_samples = 1; //number of samples per intersection

    //for BRDF PDF sampling
    //

    /*---------------------*/


    /*Light Source sampling*/

    //first find out who and where I hit
    isx = intersection_engine->GetIntersection(r);
    if(isx.object_hit == NULL)
    {
        return glm::vec3(0, 0, 0);
    }
    //traverse back the normal to prevent shadow acne
    isx.point = isx.point + epsilon*isx.normal;
    //pass in isx into EstimateDirectLighting
    //Direct_Lighting = EstimateDirectLighting(isx, num_samples, -r.direction);
    //if light source, add my light value to it
    if(isx.object_hit->material->is_light_source)
    {Emitted_Light = isx.object_hit->material->base_color;}
    else
    {Direct_Lighting = EstimateDirectLighting(isx, num_samples, -r.direction);}
    /*---------------------*/

    return Emitted_Light + Direct_Lighting;
}

glm::vec3 Integrator::EstimateDirectLighting(const Intersection &isx, unsigned int &samples_taken, const glm::vec3 &woW)
{
    //for Light source sampling
    //QList<glm::vec3> light_sample_pts;
    glm::vec3 color_temp(0,0,0);
    glm::vec3 color_final(0,0,0);
    Intersection light_sample_isx; //randomly sampled intersection on the light source's surface
    glm::vec3 wiW; // incoming ray in world frame
    Ray light_sample_ray;
    float rand1 = unif_distribution(mersenne_generator);
    float rand2 = unif_distribution(mersenne_generator);

    Intersection obstruction_test;

    //iterate through all the light sources
    for (int i = 0; i < scene->lights.count(); i++)
    {
        for(unsigned int j = 0; j < samples_taken; j++)
        {
            light_sample_isx = scene->lights[i]->GetRandISX(rand1, rand2); //take 1 sample point(intersection) on the light source for now
            wiW = light_sample_isx.point - isx.point; //ray direction going from world point to light source
            light_sample_isx.t = glm::length(wiW);
            wiW = glm::normalize(wiW);
            light_sample_ray = Ray(isx.point, wiW);//remember, the direction is from point in scene to light source
            obstruction_test = intersection_engine->GetIntersection(light_sample_ray);
            //update random point
            rand1 = unif_distribution(mersenne_generator);
            rand2 = unif_distribution(mersenne_generator);

            if (obstruction_test.object_hit == scene->lights[i])
            {
                color_temp = color_temp + CalculateEnergy(light_sample_isx, isx, light_sample_ray, woW);
            }
            else
            {
                //the ray contributes zero energy
            }
        }
        color_temp = color_temp/static_cast<float>(samples_taken); //divide by samples taken
        color_final = color_final + color_temp; // accumulate energy per high source
        color_temp = glm::vec3(0, 0, 0);// zero out color_temp for the next light source
    }

    return color_final;
}

glm::vec3 Integrator::CalculateEnergy(const Intersection &light_sample_isx, const Intersection &isx, const Ray &light_sample, const glm::vec3 &woW)
{
    glm::vec3 ray_color(0, 0, 0);
    Material* M = isx.object_hit->material; //material of point hit
    Geometry* L = light_sample_isx.object_hit; //light source
    float dummy;
     //Intersection isx_light = L->GetIntersection(light_sample);

    ray_color = ComponentMult(L->material->EvaluateScatteredEnergy(light_sample_isx, woW, -light_sample.direction, dummy), M->EvaluateScatteredEnergy(isx, woW, light_sample.direction, dummy)); // multiply the energy of the light with BRDF reflected energy
    ray_color = ray_color/L->RayPDF(light_sample_isx, light_sample)*glm::abs(glm::dot(isx.normal, light_sample.direction)); // and then do the solid angle PDF and the cosine
    ray_color = glm::clamp(ray_color, 0.0f, 1.0f);
    return ray_color;
}

void Integrator::SetDepth(unsigned int depth)
{
    max_depth = depth;
}


glm::vec3 DirectLightingIntegrator::TraceRay(Ray r, unsigned int depth)
{
    //TODO
    //Terminate if too deep//
    if(depth >= max_depth) return glm::vec3(0, 0, 0);
    //---------------------//



    //Variables We may need//

    //for the first intersection with the scene and results
    Intersection isx; // first intersection hit,
    glm::vec3 Emitted_Light(0,0,0); // stores the color value of emitted light from the point hit
    glm::vec3 Direct_Lighting(0,0,0); //stores the direct lighting
    float epsilon(0.001); //small distance;
    glm::vec3 Reflected_Light(0,0,0);// stores the color value of reflected light from the point hit
    unsigned int n_light = 50; //number of light samples
    unsigned int n_brdf = 50; //number of brdf samples

    //for BRDF PDF sampling
    //

    //---------------------//


    //Light Source sampling//

    //first find out who and where I hit
    isx = intersection_engine->GetIntersection(r);

    if(isx.object_hit == NULL)
    {
        return glm::vec3(0, 0, 0);
    }
    if(isx.object_hit->name == "Glossy_Plane_exp50")
    {
        int breakhere = 0;
    }
    //traverse back the normal to prevent shadow acne
    isx.point = isx.point + epsilon*isx.normal;
    //pass in isx into EstimateDirectLighting
    //Direct_Lighting = EstimateDirectLighting(isx, num_samples, -r.direction);
    //if light source, add my light value to it
    if(isx.object_hit->material->is_light_source)
    {Emitted_Light = isx.object_hit->material->base_color;}
    else
    {Direct_Lighting = EstimateDirectLighting(isx, n_light, n_brdf, -r.direction);}
    //---------------------//

    return Emitted_Light + Direct_Lighting;
}

glm::vec3 DirectLightingIntegrator::EstimateDirectLighting(const Intersection &isx, unsigned int &n_light, unsigned int &n_brdf, const glm::vec3 &woW)
{
    //for Light source sampling
    //QList<glm::vec3> light_sample_pts;
    glm::vec3 light_sampling_temp(0,0,0);
    glm::vec3 brdf_sampling_final(0,0,0);
    glm::vec3 light_sampling_final(0,0,0);
    glm::vec3 color_final(0,0,0);
    Intersection light_sample_isx; //randomly sampled intersection on the light source's surface
    glm::vec3 wiW; // incoming ray in world frame
    Ray light_sample_ray;
    float rand1 = unif_distribution(mersenne_generator);
    float rand2 = unif_distribution(mersenne_generator);

    Intersection obstruction_test;

    //for all light sources
    for (int i = 0; i < scene->lights.count(); i++)
    {
        //light source sampling
        for(unsigned int j = 0; j < n_light; j++)
        {
            light_sample_isx = scene->lights[i]->GetRandISX(rand1, rand2); //take 1 sample point(intersection) on the light source for now
            wiW = light_sample_isx.point - isx.point; //ray direction going from world point to light source
            light_sample_isx.t = glm::length(wiW);
            wiW = glm::normalize(wiW);
            light_sample_ray = Ray(isx.point, wiW);//remember, the direction is from point in scene to light source
            obstruction_test = intersection_engine->GetIntersection(light_sample_ray);
            //update random point
            rand1 = unif_distribution(mersenne_generator);
            rand2 = unif_distribution(mersenne_generator);

            if (obstruction_test.object_hit == scene->lights[i])
            {
                light_sampling_temp = light_sampling_temp + LightPDFEnergy(obstruction_test, isx, light_sample_ray, woW, n_light, n_brdf);
            }
            else
            {
                //the ray contributes zero energy
            }
        }


        light_sampling_temp = light_sampling_temp/static_cast<float>(n_light); //divide by samples taken
        light_sampling_final = light_sampling_final + light_sampling_temp; // accumulate energy per high source
        light_sampling_temp = glm::vec3(0, 0, 0);// zero out color_temp for the next light source
    }

    //brdf sampling
    for(unsigned int j = 0; j < n_brdf; j++)
    {
        brdf_sampling_final = brdf_sampling_final + BxDFPDFEnergy(isx, woW, n_light, n_brdf);
    }
    brdf_sampling_final = brdf_sampling_final/static_cast<float>(n_brdf);
    color_final = brdf_sampling_final + light_sampling_final;
    //clamp color_final
    glm::clamp(color_final, 0.0f, 1.0f);
    return color_final;
}

glm::vec3 DirectLightingIntegrator::LightPDFEnergy(const Intersection &light_sample_isx, const Intersection &isx, const Ray &light_sample, const glm::vec3 &woW, unsigned int n_light, unsigned int n_brdf)
{
    glm::vec3 ray_color(0, 0, 0);
    Material* M = isx.object_hit->material; //material of point hit
    Geometry* L = light_sample_isx.object_hit; //light source
    float lightPDF = L->RayPDF(light_sample_isx, light_sample);
    //if light pdf is less than zero, return no light
    if (lightPDF <= 0.0f)
    {
        return glm::vec3(0);
    }
    //get BRDFPDF and energy from the material
    float brdfPDF;
    float dummy;

    glm::vec3 M_energy(M->EvaluateScatteredEnergy(isx, woW, light_sample.direction, brdfPDF));
    glm::vec3 L_energy(L->material->EvaluateScatteredEnergy(light_sample_isx, woW, -light_sample.direction, dummy));
    float W = MIS(lightPDF, brdfPDF, n_light, n_brdf); //MIS power heuristic weighing function

    ray_color = ComponentMult(L_energy, M_energy); // multiply the energy of the light with BRDF reflected energy
    ray_color = ray_color*W/lightPDF*glm::abs(glm::dot(isx.normal, light_sample.direction)); // and then do the solid angle PDF and the cosine
    return ray_color;
}

glm::vec3 DirectLightingIntegrator::BxDFPDFEnergy(const Intersection &isx, const glm::vec3 &woW, unsigned int n_light, unsigned int n_brdf)
{
    glm::vec3 ray_color(0, 0, 0);
    glm::vec3 wiW(0, 0, 0);//this will be obtained by sampling BxDf
    Material* M = isx.object_hit->material; //material of point hit
    float brdfPDF;
    float lightPDF;
    float dummy;
    glm::vec3 M_energy(M->SampleAndEvaluateScatteredEnergy(isx, woW, wiW, brdfPDF));
    //use sampled wiW to check if I can hit the light
    Ray shadow_feeler(isx.point, wiW);
    Intersection light_isx = intersection_engine->GetIntersection(shadow_feeler);
    Geometry* L; //this holds the intersected light source

    if (light_isx.object_hit == NULL)//if ray didnt hit anything
        return ray_color;
    if (light_isx.object_hit->material->is_light_source)
    {
        L = light_isx.object_hit;
        lightPDF = L->RayPDF(light_isx, shadow_feeler);
        if (lightPDF <= 0)
        {
            return ray_color;
        }
        glm::vec3 L_energy(L->material->EvaluateScatteredEnergy(light_isx, woW, -shadow_feeler.direction, dummy));
        float W = MIS(brdfPDF, lightPDF, n_brdf, n_light);

        ray_color = ComponentMult(L_energy, M_energy);
        ray_color = ray_color*W/brdfPDF*glm::abs(glm::dot(isx.normal, shadow_feeler.direction));
        return ray_color;
    }
    else
    {
        return ray_color;
    }

}

float DirectLightingIntegrator::MIS(float f_PDF, float g_PDF, unsigned int n_f, unsigned int n_g)
{
    return (n_f*f_PDF)/(n_f*f_PDF + n_g*g_PDF);
}

