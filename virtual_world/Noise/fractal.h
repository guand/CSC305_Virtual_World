#ifndef FRACTAL
#define FRACTAL
#pragma once
#include "icg_common.h"
#include "perlin.h"

class Fractal : public Perlin {
private:
    float iqTurbulence(int i, int j, int _period)
    {
        float sum = 0.5;
        float p = _period;
        vec2 dsum = vec2(0, 0);
        for(int k = 0; k < int(this->octave); ++k)
        {
            vec3 noise = this->pseudoPerlinNoise(i, j, p);
            dsum(0) += noise(1);
            dsum(1) += noise(2);
            sum += this->exponent_array.at(k) * noise(0) / (1 + dsum.dot(dsum));
            p *= 1 / this->lacunarity;
        }
        float remainder = this->octave - int(this->octave);
        if(remainder)
        {
            vec3 noise = this->pseudoPerlinNoise(i, j, p);
            dsum(0) += noise(1);
            dsum(1) += noise(2);
            sum += remainder * noise(0) * this->exponent_array.at(int(this->octave) + 1) / (1 + dsum.dot(dsum));
        }
        return sum;
    }

    float turbulence(int i, int j, int _period)
    {
        float sum = 0;
        float p = _period;
        for(int k = 0; k < int(this->octave); ++k)
        {
            float noise = this->perlinNoise(i, j, p);
            sum += noise*this->exponent_array.at(k);
            p *= 1 / this->lacunarity;
        }

        float remainder = this->octave - int(this->octave);
        if(remainder)
            sum += remainder * this->perlinNoise(i, j, p) * this->exponent_array.at(int(this->octave) + 1);
        return sum;
    }

    float riggedNoise(int i, int j, int _period)
    {
        return .3f - abs(this->perlinNoise(i, j, _period)*1.7f);
    }

    float riggedTurbulence(int i, int j, int _period)
    {
        float sum = 0;
        float p = _period;
        for(int k = 0; k < int(this->octave); ++k)
        {
            float noise = this->riggedNoise(i, j, p);
            sum += noise*this->exponent_array.at(k);
            p *= 1 / this->lacunarity;
        }

        float remainder = this->octave - int(this->octave);
        if(remainder)
            sum += remainder * this->riggedNoise(i, j, p) * this->exponent_array.at(int(this->octave) + 1);
        return sum;
    }

public:
    Fractal(float octave, int period, int seed, int width, int height, float lacunarity, float gain)
    {
        this->octave = octave;
        this->period = period;
        this->seed = seed;
        this->width = width;
        this->height = height;
        this->lacunarity = lacunarity;
        this->gain = gain;
    }

    RGBImage setNormalNoiseMesh()
    {
        RGBImage PerlinNoise(this->width, this->height);
        for (int i = 0; i < this->width; ++i)
            for (int j = 0; j < this->height; ++j)
            {
                float noise = this->turbulence(i, j, this->period);
                PerlinNoise(i, j) = vec3(noise, noise, noise);
            }
        return PerlinNoise;
    }

    RGBImage setRiggedNoiseMesh()
    {
        RGBImage PerlinNoise(this->width, this->height);
        for (int i = 0; i < this->width; ++i)
            for (int j = 0; j < this->height; ++j)
            {
                float noise = this->riggedTurbulence(i, j, this->period);
                PerlinNoise(i, j) = vec3(noise, noise, noise);
            }
        return PerlinNoise;
    }

    RGBImage setIQNoiseMesh()
    {
        RGBImage PerlinNoise(this->width, this->height);
        for (int i = 0; i < this->width; ++i)
            for (int j = 0; j < this->height; ++j)
            {
                float noise = this->iqTurbulence(i, j, this->period);
                PerlinNoise(i, j) = vec3(noise, noise, noise);
            }
        return PerlinNoise;
    }
};


#endif // FRACTAL

