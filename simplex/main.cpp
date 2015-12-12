#include "icg_common.h"

#include "OpenGLImage/EigenVisualizer.h"

using namespace EigenVisualizer;

uint8_t perm[256] = {
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
    142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,
    203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
    74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,
    220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,
    132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,
    186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,
    59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,
    70,221,153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,
    178,185,112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,
    241,81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,
    176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,
    128,195,78,66,215,61,156,180
};

static uint8_t hash1(int32_t i) {
    return perm[static_cast<uint8_t>(i)];
}

void init(int seed)
{
     std::srand(seed);
     for(int i = 0; i < 400; ++i){
        int index = std::rand() % 255;
        int a = perm[index];
        perm[index] = perm[i%255];
        perm[i%255] = a;
     }
}


static float grad(int32_t hash, float x) {
    int32_t h = hash & 0x0F;        // Convert low 4 bits of hash code
    float grad = 1.0f + (h & 7);    // Gradient value 1.0, 2.0, ..., 8.0
    if ((h & 8) != 0) grad = -grad; // Set a random sign for the gradient
//  float grad = gradients1D[h];    // NOTE : Test of Gradient look-up table instead of the above
    return (grad * x);              // Multiply the gradient with the distance
}

static float grad(int32_t hash, float x, float y) {
    int32_t h = hash & 0x3F;  // Convert low 3 bits of hash code
    float u = h < 4 ? x : y;  // into 8 simple gradient directions,
    float v = h < 4 ? y : x;  // and compute the dot product with (x,y).
    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f*v : 2.0f*v);
}


static inline int32_t fastfloor(float fp) {
    int32_t i = (int32_t)fp;
    return (fp < i) ? (i - 1) : (i);
}

float simplex_1(float x, float y)
{
    float n0, n1, n2;   // Noise contributions from the three corners

        // Skewing/Unskewing factors for 2D
        const float F2 = 0.366025403f;  // F2 = (sqrt(3) - 1) / 2
        const float G2 = 0.211324865f;  // G2 = (3 - sqrt(3)) / 6   = F2 / (1 + 2 * K)

        // Skew the input space to determine which simplex cell we're in
        float s = (x + y) * F2;  // Hairy factor for 2D
        float xs = x + s;
        float ys = y + s;
        int32_t i = fastfloor(xs);
        int32_t j = fastfloor(ys);

        // Unskew the cell origin back to (x,y) space
        float t = static_cast<float>(i + j) * G2;
        float X0 = i - t;
        float Y0 = j - t;
        float x0 = x - X0;  // The x,y distances from the cell origin
        float y0 = y - Y0;

        // For the 2D case, the simplex shape is an equilateral triangle.
        // Determine which simplex we are in.
        int32_t i1, j1;  // Offsets for second (middle) corner of simplex in (i,j) coords
        if (x0 > y0) {   // lower triangle, XY order: (0,0)->(1,0)->(1,1)
            i1 = 1;
            j1 = 0;
        } else {   // upper triangle, YX order: (0,0)->(0,1)->(1,1)
            i1 = 0;
            j1 = 1;
        }

        // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
        // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
        // c = (3-sqrt(3))/6

        float x1 = x0 - i1 + G2;            // Offsets for middle corner in (x,y) unskewed coords
        float y1 = y0 - j1 + G2;
        float x2 = x0 - 1.0f + 2.0f * G2;   // Offsets for last corner in (x,y) unskewed coords
        float y2 = y0 - 1.0f + 2.0f * G2;

        // Calculate the contribution from the first corner
        float t0 = 0.5f - x0*x0 - y0*y0;
        if (t0 < 0.0f) {
            n0 = 0.0f;
        } else {
            t0 *= t0;
            n0 = t0 * t0 * grad(hash1(i + hash1(j)), x0, y0);
        }

        // Calculate the contribution from the second corner
        float t1 = 0.5f - x1*x1 - y1*y1;
        if (t1 < 0.0f) {
            n1 = 0.0f;
        } else {
            t1 *= t1;
            n1 = t1 * t1 * grad(hash1(i + i1 + hash1(j + j1)), x1, y1);
        }

        // Calculate the contribution from the third corner
        float t2 = 0.5f - x2*x2 - y2*y2;
        if (t2 < 0.0f) {
            n2 = 0.0f;
        } else {
            t2 *= t2;
            n2 = t2 * t2 * grad(hash1(i + 1 + hash1(j + 1)), x2, y2);
        }

        // Add contributions from each corner to get the final noise value.
        // The result is scaled to return values in the interval [-1,1].
        return 45.23065f * (n0 + n1 + n2);
}

float octave_noise_2d( const float octaves, const float persistence, const float scale, const float x, const float y ) {
    float total = 0;
    float frequency = scale;
    float amplitude = 1;

    // We have to keep track of the largest possible amplitude,
    // because each octave adds more, and we need a value in [-1, 1].
    float maxAmplitude = 0;

    for( int i=0; i < octaves; i++ ) {
        total += simplex_1( x * frequency, y * frequency ) * amplitude;

        frequency *= 2;
        maxAmplitude += amplitude;
        amplitude *= persistence;
    }

    return total / maxAmplitude;
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

float fractal(size_t octaves, float x, float y){
        float maxAmp = 0.0f;
        float output = 0.f;
        float frequency = 1 / 256.0f;
        float amplitude = 1.0f;

        for (size_t i = 0; i < octaves; i++) {
            output += amplitude * simplex_1(x * frequency, y * frequency);
            maxAmp += amplitude;
            frequency *= 2.0f;
            amplitude *= 0.5f;
        }
//        output /= maxAmp;
        return output;
}


int main(int, char**){
    int width = 512;
    int height = 512;

    //This is a demo code for using the EigenVisualizer namespace
    //TODO: change the following code so we can make a single frequency perlin noise


    int period = 256;
    float scale = 1.0;
    float frequency = 1.0f / period;
    init(45);
    RGBImage newNoise(width, height);
    for (int i = 0; i < width; ++ i)
        for (int j = 0; j < height; ++ j)
        {

            float noise = fractal(16, i, j);
//            float noise = simplex_1(i, j);
            newNoise(i, j) = vec3(noise, noise, noise);
        }
    showImage(newNoise, "Perlin Noise");
    return EXIT_SUCCESS;

}

