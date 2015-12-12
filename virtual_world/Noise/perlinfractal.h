#ifndef FRACTAL
#define FRACTAL
#pragma once
#include "icg_common.h"
#include "perlin.h"

class PerlinFractal : public Perlin {
private:
    /**
     * @brief iqTurbulence
     * @param i
     * @param j
     * @param _period
     * @return Modified IQ turbulance
     */
    float iqTurbulence(int i, int j)
    {
        float sum = 0.5;
        float p = m_period;
        vec2 dsum = vec2(0, 0);
        for(int k = 0; k < int(m_octave); ++k)
        {
            vec3 noise = pseudoPerlinNoise(i, j, p);
            dsum(0) += noise(1);
            dsum(1) += noise(2);
            sum += m_exponent_array.at(k) * noise(0) / (1 + dsum.dot(dsum));
            p *= 1 / m_lacunarity;
        }
        float remainder = m_octave - int(m_octave);
        if(remainder)
        {
            vec3 noise = pseudoPerlinNoise(i, j, p);
            dsum(0) += noise(1);
            dsum(1) += noise(2);
            sum += remainder * noise(0) * m_exponent_array.at(int(m_octave) + 1) / (1 + dsum.dot(dsum));
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
    float turbulence(int i, int j)
    {
        float sum = 0;
        float maxAmp = 0;
        float p = m_period;
        for(int k = 0; k < int(m_octave); ++k)
        {
            float noise = perlinNoise(i, j, p);
            sum += noise*m_exponent_array.at(k);
            p *= 1 / m_lacunarity;
        }

        float remainder = m_octave - int(m_octave);
        if(remainder)
            sum += remainder * perlinNoise(i, j, p) * m_exponent_array.at(int(m_octave));
        return sum;
    }

    float rigidNoise(int i, int j, int p)
    {
        return .3f - abs(perlinNoise(i, j, p));
    }

    /**
     * @brief riggedTurbulence
     * @param i
     * @param j
     * @param _period
     * @return rigid turbulence
     */
    float rigidTurbulence(int i, int j)
    {
        float sum = 0;
        float p = m_period;
        for(int k = 0; k < int(m_octave); ++k)
        {
            float noise = rigidNoise(i, j, p);
            sum += noise*m_exponent_array.at(k);
            p *= 1 / m_lacunarity;
        }

        float remainder = m_octave - int(m_octave);
        if(remainder)
            sum += remainder * perlinNoise(i, j, p) * m_exponent_array.at(int(m_octave));
        return sum;
    }

    float hybridNoise(int i, int j, int p)
    {
        return perlinNoise(i, j, p) + m_offset;
    }

    float hybridTurbulence(int i, int j)
    {
        float sum = 0;
        float p = m_period;
        float maxAmp = 0;
        float result = hybridNoise(i, j, p) * m_exponent_array.at(0);
        p *= 1 / m_lacunarity;
        float weight = result;
        for(int k = 1; k < int(m_octave); ++k)
        {
            if( weight > 1.0 ) weight = 1.0;

            float noise = hybridNoise(i, j, p);
            float signal = noise*m_exponent_array.at(k);
            sum += weight*signal;
            p *= 1 / m_lacunarity;
            weight *= signal;
        }

        float remainder = m_octave - int(m_octave);
        if(remainder)
            sum += remainder * perlinNoise(i, j, p) * m_exponent_array.at(int(m_octave));
        return sum;
    }

public:
    PerlinFractal(float octave, int period, int seed, int width, int height, float lacunarity, float gain, float offset)
    {
        m_octave = octave;
        m_period = period;
        m_seed = seed;
        m_width = width;
        m_height = height;
        m_lacunarity = lacunarity;
        m_gain = gain;
        m_offset = offset;
    }

    RGBImage setNormalNoise()
    {
        RGBImage PerlinNoise(m_width, m_height);
        for (int i = 0; i < m_width; ++i)
            for (int j = 0; j < m_height; ++j)
            {
                float noise = turbulence(i, j);
                PerlinNoise(i, j) = vec3(noise, noise, noise);
            }
        return PerlinNoise;
    }

    RGBImage setRigidNoise()
    {
        RGBImage PerlinNoise(m_width, m_height);
        for (int i = 0; i < m_width; ++i)
            for (int j = 0; j < m_height; ++j)
            {
                float noise = rigidTurbulence(i, j);
                PerlinNoise(i, j) = vec3(noise, noise, noise);
            }
        return PerlinNoise;
    }

    RGBImage setIQNoise()
    {
        RGBImage PerlinNoise(m_width, m_height);
        for (int i = 0; i < m_width; ++i)
            for (int j = 0; j < m_height; ++j)
            {
                float noise = iqTurbulence(i, j);
                PerlinNoise(i, j) = vec3(noise, noise, noise);
            }
        return PerlinNoise;
    }

    RGBImage setHybridNoise()
    {
        RGBImage PerlinNoise(m_width, m_height);
        for (int i = 0; i < m_width; ++i)
            for (int j = 0; j < m_height; ++j)
            {
                float noise = hybridTurbulence(i, j);
                PerlinNoise(i, j) = vec3(noise, noise, noise);
            }
        return PerlinNoise;
    }
};


#endif // FRACTAL

