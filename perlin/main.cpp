#include "icg_common.h"

#include "OpenGLImage/EigenVisualizer.h"

using namespace EigenVisualizer;

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

RGBImage baseNoise(int width, int height)
{
    RGBImage _base(width, height);
    std::srand(134);
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

float perlinNoise(int i, int j, int width, int height, int period, const RGBImage& base)
{
    float frequency = 1.0f / period;
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


RGBImage originalPerlinNoise(int width, int height, int period, const RGBImage& base)
{
    RGBImage _perlin(width, height);
    float frequency = 1.0f / period;
    for(int i = 0; i < width; ++i){
        for(int j = 0; j < height; ++j){
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
            _perlin(i, j) = vec3(noise, noise, noise);
        }
    }
    return _perlin;
}


vec3 pseudoPerlinNoise(int i, int j, int width, int height, int period, const RGBImage& base)
{
    float frequency = 1.0f / period;
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

    float f2x = f2(dx) * ( (t-s) + (s-t-v+u)* fy);
    float f2y = f2(dy) * ( (v-s) + (s-t-v+u)* fx);

    return vec3(noise, noise, noise);
}

float fBM(int i, int j, const RGBImage& perlin, float H, float lacunarity, float octaves)
{
    float sum = 0;
    float amp = 1.0;
    float freq = 1.0f;
    std::vector<float> exponent_array;
    for(int k = 0; k < octaves; ++k)
    {
        exponent_array.push_back(pow(freq, -H));
        freq *= lacunarity;
    }

    for(int k = 0; k < octaves; ++k)
    {
        float n = perlin(i%perlin.rows(), j%perlin.cols())(0);
        sum += n* exponent_array.at(k);
        i *= lacunarity;
        j *= lacunarity;
    }
    return sum;
}

float turbulence(int period, int octave, int i, int j, int width, int height, const RGBImage& base, float lacunarity = 2.0, float gain = 0.5)
{
    float sum = 0;
    float per = period;
    float amp = 1.0;
    float freq = 1.0f;
    std::vector<float> exponent_array;
    for(int k = 0; k < octave; ++k)
    {
        exponent_array.push_back(pow(freq, -0.25));
        freq *= lacunarity;
    }

    for(int k = 0; k < octave; ++k)
    {
        float n = perlinNoise(i, j, width, height, per, base);
        sum += n* exponent_array.at(k);
        per *= 1 / lacunarity;
    }
    return sum;
}

float iqTurbulence(int period, int octave, int i, int j, int width, int height, const RGBImage& base, float lacunarity = 2.0, float gain = 0.5)
{
    float sum = 0;
    float per = period;
    float amp = 1.0;
    vec2 dsum = vec2(0, 0);
    for(int k = 0; k < octave; ++k)
    {
        vec3 noise = pseudoPerlinNoise(i, j, width, height, per, base);
        dsum(0) += noise(1);
        dsum(1) += noise(2);
        sum += amp * noise(0) / (1 + dsum.dot(dsum));
        per *= 1 / lacunarity;
        amp *= gain;
    }
    return sum;
}

int main(int, char**){
    int width = 512;
    int height = 512;

    //This is a demo code for using the EigenVisualizer namespace
    //TODO: change the following code so we can make a single frequency perlin noise
    RGBImage base = baseNoise(width, height);


    int period = 256;
    float frequency = 1.0f / period;
//     RGBImage PerlinNoise(width, height);
//    for(int i = 0; i < 4; ++i){
//        PerlinNoise += originalPerlinNoise(width, height, period, base);
//        period = period / 2;
//    }
    RGBImage newNoise(width, height);
    for (int i = 0; i < width; ++ i)
        for (int j = 0; j < height; ++ j)
        {

//            float noise = iqTurbulence(period, 4, i, j, width, height, base);
//            float noise = fBM(i, j, PerlinNoise, 0.25, 2.0, 4.0);
            float noise = turbulence(period, 5, i, j, width, height, base);
//            float noise = smoothNoise(i, j);
            newNoise(i, j) = vec3(noise, noise, noise);
        }
    showImage(newNoise, "Perlin Noise");
//    showImage(base, "Base Nose");

    return EXIT_SUCCESS;

}

