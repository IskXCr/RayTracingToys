#include <iostream>
#include <limits>
#include <random>

#include "sphere.hpp"
#include "hitable_list.hpp"
#include "camera.hpp"

hitable *random_scene()
{
    static std::default_random_engine rng;
    static std::uniform_real_distribution<float> dist(0.0, 1.0);

    int n = 500;
    hitable **list = new hitable *[n + 1];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for (int a = -3; a < 3; a++)
    {
        for (int b = -3; b < 3; b++)
        {
            float choose_mat = dist(rng);
            vec3 center(a + 0.9 * dist(rng), 0.2, b + 0.9 * dist(rng));
            if ((center - vec3(4, 0.2, 0)).length() > 0.9)
            {
                if (choose_mat < 0.8) // diffuse
                {
                    list[i++] = new sphere(center, 0.2, new lambertian(vec3(dist(rng) * dist(rng), dist(rng) * dist(rng), dist(rng) * dist(rng))));
                }
                else if (choose_mat < 0.95) // metal
                {
                    list[i++] = new sphere(center, 0.2, new metal(vec3(0.5 * (1 + dist(rng)), 0.5 * (1 + dist(rng)), 0.5 * (1 + dist(rng))), 0.5 * dist(rng)));
                }
                else // glass
                {
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }
    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new hitable_list(list, i);
}

vec3 color(const ray &r, hitable *world, int depth)
{
    hit_record rec;
    if (world->hit(r, 0.001, std::numeric_limits<float>().max(), rec))
    {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        {
            return attenuation * color(scattered, world, depth + 1);
        }
        else
        {
            return vec3(0, 0, 0);
        }
    }
    else
    {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

int main()
{
    std::default_random_engine rng;
    std::uniform_real_distribution<float> dist(0.0, 1.0);
    /* print PPM header */
    int nx = 800; // length of x direction
    int ny = 400; // length of y direction
    int ns = 100;

    std::cout << "P3\n"
              << nx << " " << ny << "\n255\n";

    hitable *world = random_scene();
    std::cerr << "scene gen completed.\n";

    vec3 lookfrom(9, 5, 17);
    vec3 lookat(0, 2, -1);
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 0.4;

    camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus);

    for (int j = ny - 1; j >= 0; j--)
    {
        std::cerr << "iteration: generating row=" << j << ".\n";
        for (int i = 0; i < nx; i++)
        {
            vec3 col(0, 0, 0);
#pragma omp parallel for
            for (int s = 0; s < ns; s++)
            {
                float u = float(i + dist(rng)) / float(nx);
                float v = float(j + dist(rng)) / float(ny);
                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += color(r, world, 0);
            }

            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2])); // gamma correction

            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}