#ifndef NOISE
#define NOISE

#pragma once
#include "icg_common.h"

class Noise {
protected:
    typedef Eigen::Matrix<Eigen::Vector3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RGBImage;
    float octave;
    int period;
    int seed;
    int width;
    int height;
    float lacunarity;
    float gain;
    float offset;
    std::vector<float> exponent_array;
    RGBImage base;

private:
    //return a random floating point number between [0, 1]
    float rand0_1()
    {
        return ((float) std::rand())/((float) RAND_MAX);
    }

public:
    void init() {
        float _frequency = 1.0f;
        for(int i = 0; i < (this->octave + 1); ++i)
        {
            this->exponent_array.push_back(pow(_frequency, -1*(this->gain)));
            _frequency *= this->lacunarity;
        }

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
};

#endif // NOISE

