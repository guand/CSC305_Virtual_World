#pragma once
#include "icg_common.h"
#include "Mesh.h"

class Perlin{
private:
    typedef Eigen::Matrix<Eigen::Vector3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RGBImage;
    int octave;
    int period;
    int seed;
    int width;
    int height;
    float lacunarity;
    float gain;
    RGBImage base;

private:
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

    float f2(float t)
    {
        float t_3 = t * t * t;
        return 30 * t * t * t_3 - 60 * t * t_3 + 30 * t_3;
    }

    float perlinNoise(int i, int j, int period)
    {
        float frequency = 1.0f / period;
        int left = (i / period) * period;
        int right = (left + period) % this->width;
        float dx = (i - left) * frequency;

        int top = (j / period) * period;
        int bottom = (top + period) % this->height;
        float dy = (j - top) * frequency;

        vec2 a(dx, -dy);
        vec2 b(dx - 1, -dy);
        vec2 c(dx - 1, 1 - dy);
        vec2 d(dx, 1 - dy);

        vec3 topleft = this->base(left, top);
        float s = topleft(0) * a(0) + topleft(1) * a(1);
        vec3 topright = this->base(right, top);
        float t = topright(0) * b(0) + topright(1) * b(1);
        vec3 bottomleft = this->base(left, bottom);
        float u = bottomleft(0) * d(0) + bottomleft(1) * d(1);
        vec3 bottomright = this->base(right, bottom);
        float v = bottomright(0) * c(0) + bottomright(1) * c(1);

        float fx = this->f(dx);
        float fy = this->f(dy);

        float st = this->mix(s, t, fx);
        float uv = this->mix(u, v, fx);
        float noise = this->mix(st, uv, fy);
        return noise;
    }

    vec3 pseudoPerlinNoise(int i, int j, int period)
    {
        float frequency = 1.0f / period;
        int left = (i / period) * period;
        int right = (left + period) % this->width;
        float dx = (i - left) * frequency;

        int top = (j / period) * period;
        int bottom = (top + period) % this->height;
        float dy = (j - top) * frequency;

        vec2 a(dx, -dy);
        vec2 b(dx - 1, -dy);
        vec2 c(dx - 1, 1 - dy);
        vec2 d(dx, 1 - dy);

        vec3 topleft = this->base(left, top);
        float s = topleft(0) * a(0) + topleft(1) * a(1);
        vec3 topright = this->base(right, top);
        float t = topright(0) * b(0) + topright(1) * b(1);
        vec3 bottomleft = this->base(left, bottom);
        float u = bottomleft(0) * d(0) + bottomleft(1) * d(1);
        vec3 bottomright = this->base(right, bottom);
        float v = bottomright(0) * c(0) + bottomright(1) * c(1);

        float fx = this->f(dx);
        float fy = this->f(dy);

        float st = this->mix(s, t, fx);
        float uv = this->mix(u, v, fx);
        float noise = this->mix(st, uv, fy);

        float f2x = this->f2(dx) * ( (t-s) + (s-t-v+u)* fy);
        float f2y = this->f2(dy) * ( (v-s) + (s-t-v+u)* fx);

        return vec3(noise, f2x, f2y);
    }

    float iqTurbulence(int i, int j, int _period)
    {
        float sum = 0.5;
        float p = _period;
        float amplitude = 1.0;
        vec2 dsum = vec2(0, 0);
        for(int k = 0; k < this->octave; ++k)
        {
            vec3 noise = this->pseudoPerlinNoise(i, j, p);
            dsum(0) += noise(1);
            dsum(1) += noise(2);

            sum += amplitude * noise(0) / (1 + dsum.dot(dsum));
            p *= 1 / this->lacunarity;
            amplitude *= this->gain;
        }
        return sum;
    }

    float turbulence(int i, int j, int _period)
    {
        float sum = 0;
        float p = _period;
        float amplitude = 1.0;
        for(int k = 0; k < this->octave; ++k)
        {
            float noise = this->perlinNoise(i, j, p);
            sum += noise*amplitude;
            p *= 1 / this->lacunarity;
            amplitude *= this->gain;
        }
        return sum;
    }

    float riggedNoise(int i, int j, int _period)
    {
        return .3f - abs(this->perlinNoise(i, j, _period));
    }

    float riggedTurbulence(int i, int j, int _period)
    {
        float sum = 0;
        float p = _period;
        float amplitude = 1.0;
        for(int k = 0; k < this->octave; ++k)
        {
            float noise = this->riggedNoise(i, j, p);
            sum += noise*amplitude;
            p *= 1 / this->lacunarity;
            amplitude *= this->gain;
        }
        return sum;
    }


public:
    Perlin(int octave, int period, int seed, int width, int height, float lacunarity, float gain)
    {
        this->octave = octave;
        this->period = period;
        this->seed = seed;
        this->width = width;
        this->height = height;
        this->lacunarity = lacunarity;
        this->gain = gain;
    }

    void setBase()
    {
        std::srand(this->seed);
        RGBImage _base(this->width, this->height);
        for(int i = 0; i < this->width; ++i)
        {
            for(int j = 0; j < this->height; ++j)
            {
                vec3 randGradientVec;
                randGradientVec(0) = cos(2 * M_PI * this->rand0_1());
                randGradientVec(1) = sin(2 * M_PI * this->rand0_1());
                randGradientVec(2) = 0;
                _base(i, j) = randGradientVec;
            }
        }
        this->base = _base;
    }

    void setImage(RGBImage & image, Mesh & mesh)
    {
        unsigned int _width = image.rows();
        unsigned int _height = image.cols();
        //After the openGL context is set up, copy data into an openGL texture
        mesh.loadTextureRGB32F(image.data(), _width, _height);
    }

    void setNormalNoiseMesh(Mesh & mesh)
    {
        RGBImage PerlinNoise(this->width, this->height);
        for (int i = 0; i < this->width; ++i)
            for (int j = 0; j < this->height; ++j)
            {
                float noise = this->turbulence(i, j, this->period);
                PerlinNoise(i, j) = vec3(noise, noise, noise);
            }
        setImage(PerlinNoise, mesh);
    }

    void setRiggedNoiseMesh(Mesh & mesh)
    {
        RGBImage PerlinNoise(this->width, this->height);
        for (int i = 0; i < this->width; ++i)
            for (int j = 0; j < this->height; ++j)
            {
                float noise = this->riggedTurbulence(i, j, this->period);
                PerlinNoise(i, j) = vec3(noise, noise, noise);
            }
        setImage(PerlinNoise, mesh);
    }

    void setIQNoiseMesh(Mesh & mesh)
    {
        RGBImage PerlinNoise(this->width, this->height);
        for (int i = 0; i < this->width; ++i)
            for (int j = 0; j < this->height; ++j)
            {
                float noise = this->iqTurbulence(i, j, this->period);
                PerlinNoise(i, j) = vec3(noise, noise, noise);
            }
        setImage(PerlinNoise, mesh);
    }

};
