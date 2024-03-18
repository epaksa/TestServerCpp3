#include "Random.h"

Random::Random() : _dist(0, INT32_MAX), _mt(_rd())
{
}

const int Random::Get(const int min, const int max)
{
    const int real_max = max + 1;

    return (real_max <= min) ? min : min + (_dist(_mt) % (real_max - min));
}
