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

public:
    virtual void init() = 0;
};

#endif // NOISE

