#include "vse_math.h"

uint32_t vse_math_clamp_uint(uint32_t value, uint32_t min, uint32_t max) {
    const uint32_t t = value < min ? min : value;
    return t > max ? max : t;
}