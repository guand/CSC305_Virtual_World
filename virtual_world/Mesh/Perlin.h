#pragma once
#include "icg_common.h"
#include "Mesh.h"

class Perlin{
private:
    typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> GreyLevelImage;
    typedef Eigen::Matrix<Eigen::Vector3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RGBImage;
    int _octave = 9;

private:
    RGBImage _baseNoise(int width, int height)
    {
        RGBImage _base(width, height);

        std::srand(9);
        for (int i = 0; i < width; ++ i)
        {
            for (int j = 0; j < height; ++ j)
            {
                //we use vec3 instead of vec2 but set the last component to zero
                vec3 randGradientVec;
                randGradientVec(0) = cos(2 * M_PI * rand0_1());
                randGradientVec(1) = sin(2 * M_PI * rand0_1());
                randGradientVec(2) = 0;
                _base(i, j) = randGradientVec;
            }
        }
        return _base;
    }

    float _perlinNoise(int i, int j, int width, int height, float frequency, int period, const RGBImage& base)
    {
        int left = (i / period) * period;
        int right = (left + period) % width;
        float dx = (i - left) * frequency;

        int top = (j / period) * period;
        int bottom = (top + period) % height;
        float dy = (j - top) * frequency;

        vec2 a(dx, -dy);
        vec2 b(dx - 1, -dy);
        vec2 c(dx - 1, 1 - dy);
        vec2 d(dx, 1 - dy);

        vec3 topleft = base(left, top);
        float s = topleft(0) * a(0) + topleft(1) * a(1);
        vec3 topright = base(right, top);
        float t = topright(0) * b(0) + topright(1) * b(1);
        vec3 bottomleft = base(left, bottom);
        float u = bottomleft(0) * d(0) + bottomleft(1) * d(1);
        vec3 bottomright = base(right, bottom);
        float v = bottomright(0) * c(0) + bottomright(1) * c(1);

        float fx = f(dx);
        float fy = f(dy);

        float st = mix(s, t, fx);
        float uv = mix(u, v, fx);
        float noise = mix(st, uv, fy);
        return noise;
    }

    float _turbulence(float frequency, int period, int octave, int i, int j, int width, int height, const RGBImage& base, float lacunarity = 2.0, float gain = 0.5)
    {
        float sum = 0;
        float _frequency = frequency;
        float _period = period;
        float amplitude = 1.0;
        for(int k = 0; k < octave; ++k)
        {
            float noise = _perlinNoise(i, j, width, height, _frequency, _period, base);
            sum += noise*amplitude;
            _frequency *= lacunarity;
            _period *= 0.5;
            amplitude *= gain;
        }
        return sum;
    }

public:
    //return a random floating point number between [0, 1]
    float rand0_1()
    {
        return ((float) std::rand())/((float) RAND_MAX);
    }

    float mix(float x, float y, float alpha)
    {
        return y * alpha + x * (1.0f - alpha);
    }


    float f(float t)
    {
        float t_3 = t * t * t;
        return 6 * t * t * t_3 - 15 * t * t_3 + 10 * t_3;
    }

    void setImage(GreyLevelImage & image, Mesh & mesh)
    {
        unsigned int width = image.rows();
        unsigned int height = image.cols();

        //After the openGL context is set up, copy data into an openGL texture
        mesh.loadTextureR32F(image.data(), width, height);
    }

    void setImage(RGBImage & image, Mesh & mesh)
    {
        unsigned int width = image.rows();
        unsigned int height = image.cols();

        //After the openGL context is set up, copy data into an openGL texture
        mesh.loadTextureRGB32F(image.data(), width, height);
    }

    void noise(int width, int height, Mesh & mesh)
    {
        //This is a demo code for using the EigenVisualizer namespace
        //TODO: change the following code so we can make a single frequency perlin noise
        RGBImage base = _baseNoise(width, height);

        int period = 512;
        float frequency = 1.0f / period;
        RGBImage PerlinNoise(width, height);
        for (int i = 0; i < width; ++ i)
            for (int j = 0; j < height; ++ j)
            {
                float noise = _turbulence(frequency, period, _octave, i, j, width, height, base);
                PerlinNoise(i, j) = vec3(noise, noise, noise);
            }
        setImage(PerlinNoise, mesh);
    }

};
