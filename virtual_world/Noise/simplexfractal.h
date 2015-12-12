#ifndef SIMPLEXFRACTAL
#define SIMPLEXFRACTAL
#include "icg_common.h"
#include "simplex.h"

class SimplexFractal : public Simplex {

private:
    float fractal(float i, float j){
            float maxAmp = 0.0f;
            float value = 0.f;
            float frequency = 1 / float(m_period);

            for (size_t k = 0; k < int(m_octave); ++k) {
                value += m_exponent_array.at(k) * simplex(i * frequency, j * frequency);
                frequency *= m_lacunarity;
            }

            float remainder = m_octave - int(m_octave);
            if(remainder) value += remainder * simplex(i * frequency, j * frequency) * m_exponent_array.at(int(m_octave) + 1);

            for(int k = 0; k < m_exponent_array.size(); ++k)
            {
                maxAmp += m_exponent_array.at(k);
            }
            value /= maxAmp;
            return value;
    }

    float hybridFractal(float i, float j){
        float maxAmp = 0.0f;
        float value = 0.f;
        float signal;
        float frequency = 1 / float(m_period);
        value += m_exponent_array.at(0) * (simplex(i * frequency, j * frequency) + m_offset);
        float weight = value;
        frequency *= m_lacunarity;
        for (size_t k = 1; k < int(m_octave); ++k) {
            if(weight > 1.0) weight = 1.0;
            signal = m_exponent_array.at(k) * (simplex(i * frequency, j * frequency) + m_offset);
            value += weight * signal;
            frequency *= m_lacunarity;
        }
        float remainder = m_octave - int(m_octave);
        if(remainder) value += remainder * simplex(i * frequency, j * frequency) * m_exponent_array.at(int(m_octave) + 1);
        for(int k = 0; k < m_exponent_array.size(); ++k)
        {
            maxAmp += m_exponent_array.at(k);
        }
        value /= maxAmp;
        return value;
    }


    float rigidFractal(float i, float j){
        float maxAmp = 0.0f;
        float value = 0.f;
        float signal;
        float frequency = 1 / float(m_period);
        value += m_exponent_array.at(0) * ((.7-abs(simplex(i * frequency, j * frequency))) + m_offset);
        float weight = value;
        frequency *= m_lacunarity;
        for (size_t k = 1; k < int(m_octave); ++k) {
            if(weight > 1.0) weight = 1.0;
            signal = m_exponent_array.at(k) * ((.7-abs(simplex(i * frequency, j * frequency))) + m_offset);
            value += weight * signal;
            frequency *= m_lacunarity;
        }
        float remainder = m_octave - int(m_octave);
        if(remainder) value += remainder * simplex(i * frequency, j * frequency) * m_exponent_array.at(int(m_octave) + 1);
        for(int k = 0; k < m_exponent_array.size(); ++k)
        {
            maxAmp += m_exponent_array.at(k);
        }
        value /= maxAmp;
        return value;
    }

public:
    SimplexFractal(float octave, int period, int seed, int width, int height, float lacunarity, float gain, float offset)
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
        RGBImage SimplexNoise(m_width, m_height);
        for (int i = 0; i < m_width; ++i)
            for (int j = 0; j < m_height; ++j)
            {
                float noise = fractal(i, j);
                SimplexNoise(i, j) = vec3(noise, noise, noise);
            }
        return SimplexNoise;
    }

    RGBImage setHybridNoise()
    {
        RGBImage SimplexNoise(m_width, m_height);
        for (int i = 0; i < m_width; ++i)
            for (int j = 0; j < m_height; ++j)
            {
                float noise = hybridFractal(i, j);
                SimplexNoise(i, j) = vec3(noise, noise, noise);
            }
        return SimplexNoise;
    }

    RGBImage setRigidNoise()
    {
        RGBImage SimplexNoise(m_width, m_height);
        for (int i = 0; i < m_width; ++i)
            for (int j = 0; j < m_height; ++j)
            {
                float noise = rigidFractal(i, j);
                SimplexNoise(i, j) = vec3(noise, noise, noise);
            }
        return SimplexNoise;
    }

};


#endif // SIMPLEXFRACTAL

