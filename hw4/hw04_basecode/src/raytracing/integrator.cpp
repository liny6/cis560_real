#include <raytracing/integrator.h>


Integrator::Integrator():
    max_depth(5)
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
    float epsilon(0.00001); //small distance;
    glm::vec3 Reflected_Light(0,0,0);// stores the color value of reflected light from the point hit
    unsigned int num_samples = 20; //number of samples per intersection

    //for BRDF PDF sampling
    //

    /*---------------------*/


    /*Light Source sampling*/

    //first find out who and where I hit
    isx = intersection_engine->GetIntersection(r);
    //traverse back the normal to prevent shadow acne
    isx.point = isx.point - epsilon*isx.normal;
    //pass in isx into EstimateDirectLighting
    Direct_Lighting = EstimateDirectLighting(isx, num_samples, -r.direction);
    //if light source, add my light value to it
    if(isx.object_hit->material->is_light_source) Emitted_Light = isx.object_hit->material->base_color;
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
    float rand1;
    float rand2;

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
    Ray reverse_light_sample(light_sample.origin, -light_sample.direction);//this ray is for calculating PDF cuz

    ray_color = ComponentMult(L->material->base_color, M->EvaluateScatteredEnergy(isx, woW, light_sample.direction)); // multiply the energy of the light with BRDF reflected energy
    ray_color = ray_color/L->RayPDF(light_sample_isx, reverse_light_sample)*glm::abs(glm::dot(isx.normal, light_sample.direction)); // and then do the solid angle PDF and the cosine

    return ray_color;
}

void Integrator::SetDepth(unsigned int depth)
{
    max_depth = depth;
}
