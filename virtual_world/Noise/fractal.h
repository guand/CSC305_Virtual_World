#ifndef FRACTAL
#define FRACTAL
#pragma once
#include "icg_common.h"
#include "perlin.h"

class Fractal : public Perlin {
private:
    /**
     * @brief iqTurbulence
     * @param i
     * @param j
     * @param _period
     * @return Modified IQ turbulance
     */
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

    /**
     * @brief turbulence
     * @param i
     * @param j
     * @param _period
     * @return Basic turbulence function
     */
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

    float rigidNoise(int i, int j, int _period)
    {
        return .3f - abs(this->perlinNoise(i, j, _period));
    }

    /**
     * @brief riggedTurbulence
     * @param i
     * @param j
     * @param _period
     * @return rigid turbulence
     */
    float rigidTurbulence(int i, int j, int _period)
    {
        float sum = 0;
        float p = _period;
        for(int k = 0; k < int(this->octave); ++k)
        {
            float noise = this->rigidNoise(i, j, p);
            sum += noise*this->exponent_array.at(k);
            p *= 1 / this->lacunarity;
        }

        float remainder = this->octave - int(this->octave);
        if(remainder)
            sum += remainder * this->perlinNoise(i, j, p) * this->exponent_array.at(int(this->octave) + 1);
        return sum;
    }

    float hybridNoise(int i, int j, int _period)
    {
        return this->perlinNoise(i, j, _period) + this->offset;
    }

    float hybridTurbulence(int i, int j, int _period)
    {
        float sum = 0;
        float p = _period;
        float result = this->hybridNoise(i, j, p) * this->exponent_array.at(0);
        float weight = result;
        for(int k = 1; k < int(this->octave); ++k)
        {
            if( weight > 1.0 ) weight = 1.0;

            float noise = this->hybridNoise(i, j, p);
            sum += noise*this->exponent_array.at(k)*weight;
            p *= 1 / this->lacunarity;
            weight *= sum;
        }

        float remainder = this->octave - int(this->octave);
        if(remainder)
            sum += remainder * this->perlinNoise(i, j, p) * this->exponent_array.at(int(this->octave) + 1);
        return sum;
    }

public:
    Fractal(float octave, int period, int seed, int width, int height, float lacunarity, float gain, float offset)
    {
        this->octave = octave;
        this->period = period;
        this->seed = seed;
        this->width = width;
        this->height = height;
        this->lacunarity = lacunarity;
        this->gain = gain;
        this->offset = offset;
    }

    RGBImage setNormalNoise()
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

    RGBImage setRigidNoise()
    {
        RGBImage PerlinNoise(this->width, this->height);
        for (int i = 0; i < this->width; ++i)
            for (int j = 0; j < this->height; ++j)
            {
                float noise = this->rigidTurbulence(i, j, this->period);
                PerlinNoise(i, j) = vec3(noise, noise, noise);
            }
        return PerlinNoise;
    }

    RGBImage setIQNoise()
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

    RGBImage setHybridNoise()
    {
        RGBImage PerlinNoise(this->width, this->height);
        for (int i = 0; i < this->width; ++i)
            for (int j = 0; j < this->height; ++j)
            {
                float noise = this->hybridTurbulence(i, j, this->period);
                PerlinNoise(i, j) = vec3(noise, noise, noise);
            }
        return PerlinNoise;
    }
};


#endif // FRACTAL

