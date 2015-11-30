#ifndef NOISE
#define NOISE

#pragma once
#include "icg_common.h"

class Noise {
protected:
    typedef Eigen::Matrix<Eigen::Vector3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RGBImage;
    float m_octave;
    int m_period;
    int m_seed;
    int m_width;
    int m_height;
    float m_lacunarity;
    float m_gain;
    float m_offset;
    std::vector<float> m_exponent_array;
    RGBImage m_base;

private:
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

#endif // NOISE

