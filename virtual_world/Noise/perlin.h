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

//        float f2x = this->f2(dx) * ( (t-s) + (s-t-v+u)* fy);
//        float f2y = this->f2(dy) * ( (v-s) + (s-t-v+u)* fx);

        return vec3(noise, noise, noise);
    }
};

#endif // PERLIN
