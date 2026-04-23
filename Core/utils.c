#include "utils.h"

float_t fClamp(float_t value, float_t min_value, float_t max_value)
{
    if (value < min_value)
        return min_value;

    if (value > max_value)
        return max_value;

    return value;
}

float_t fAbs(float_t value)
{
    if (value < 0.0f)
    {
        return -value;
    }

    return value;
}