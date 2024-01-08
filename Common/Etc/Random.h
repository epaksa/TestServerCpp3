#pragma once

#include <random>

class Random
{
public:
    Random();

    const int Get(const int min, const int max);

private:
    std::random_device _rd;
    std::mt19937 _mt;
    std::uniform_int_distribution<int> _dist;
};

