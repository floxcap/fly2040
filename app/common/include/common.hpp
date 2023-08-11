#ifndef _COMMON_HPP
#define _COMMON_HPP

#include <cmath>

inline float roundp(float v, uint8_t prec = 2)
{
    for (uint8_t i=0; i<prec; i++)
    {
        v = v * 10;
    }
    v = std::round(v);
    for (uint8_t i=0; i<prec; i++)
    {
        v = v / 10;
    }

    return v;
}

#endif //_COMMON_HPP
