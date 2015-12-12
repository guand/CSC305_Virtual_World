#ifndef PERLIN
#define PERLIN

#pragma once
#include "icg_common.h"
#include "../Mesh/mesh.h"
#include "noise.h"

class Perlin : public Noise {
protected:
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
        int right = (left + period) % m_width;
        float dx = (i - left) * frequency;

        int top = (j / period) * period;
        int bottom = (top + period) % m_height;
        float dy = (j - top) * frequency;

        vec2 a(dx, -dy);
        vec2 b(dx - 1, -dy);
        vec2 c(dx - 1, 1 - dy);
        vec2 d(dx, 1 - dy);

        vec3 topleft = m_base(left, top);
        float s = topleft(0) * a(0) + topleft(1) * a(1);
        vec3 topright = m_base(right, top);
        float t = topright(0) * b(0) + topright(1) * b(1);
        vec3 bottomleft = m_base(left, bottom);
        float u = bottomleft(0) * d(0) + bottomleft(1) * d(1);
        vec3 bottomright = m_base(right, bottom);
        float v = bottomright(0) * c(0) + bottomright(1) * c(1);

        float fx = f(dx);
        float fy = f(dy);

        float st = mix(s, t, fx);
        float uv = mix(u, v, fx);
        float noise = mix(st, uv, fy);
        return noise;
    }

    vec3 pseudoPerlinNoise(int i, int j, int period)
    {
        float frequency = 1.0f / period;
        int left = (i / period) * period;
        int right = (left + period) % m_width;
        float dx = (i - left) * frequency;

        int top = (j / period) * period;
        int bottom = (top + period) % m_height;
        float dy = (j - top) * frequency;

        vec2 a(dx, -dy);
        vec2 b(dx - 1, -dy);
        vec2 c(dx - 1, 1 - dy);
        vec2 d(dx, 1 - dy);

        vec3 topleft = m_base(left, top);
        float s = topleft(0) * a(0) + topleft(1) * a(1);
        vec3 topright = m_base(right, top);
        float t = topright(0) * b(0) + topright(1) * b(1);
        vec3 bottomleft = m_base(left, bottom);
        float u = bottomleft(0) * d(0) + bottomleft(1) * d(1);
        vec3 bottomright = m_base(right, bottom);
        float v = bottomright(0) * c(0) + bottomright(1) * c(1);

        float fx = f(dx);
        float fy = f(dy);

        float st = mix(s, t, fx);
        float uv = mix(u, v, fx);
        float noise = mix(st, uv, fy);

//        float f2x = this->f2(dx) * ( (t-s) + (s-t-v+u)* fy);
//        float f2y = this->f2(dy) * ( (v-s) + (s-t-v+u)* fx);

        return vec3(noise, noise, noise);
    }

    //return a random floating point number between [0, 1]
    float rand0_1()
    {
        return ((float) std::rand())/((float) RAND_MAX);
    }

public:
    void init() {
        float _frequency = 1.0f;
        for(int i = 0; i < (m_octave + 1); ++i)
        {
            m_exponent_array.push_back(pow(_frequency, -1*(m_gain)));
            _frequency *= m_lacunarity;
        }

        std::srand(m_seed);
        RGBImage _base(m_width, m_height);
        for(int i = 0; i < m_width; ++i)
        {
            for(int j = 0; j < m_height; ++j)
            {
                vec3 randGradientVec;
                randGradientVec(0) = cos(2 * M_PI * rand0_1());
                randGradientVec(1) = sin(2 * M_PI * rand0_1());
                randGradientVec(2) = 0;
                _base(i, j) = randGradientVec;
            }
        }
        m_base = _base;
    }
};

#endif // PERLIN
