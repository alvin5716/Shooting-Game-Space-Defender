#ifndef SPREADRNG_H
#define SPREADRNG_H

#include <vector>
#include <random>

class SpreadRNG
{
public:
    SpreadRNG() = delete;
    SpreadRNG(int lower, int upper);
    void reset();
    void reset(int lower, int upper);
    int generate();
private:
    int lower, upper;
    std::default_random_engine* generator;
    std::vector<double>* rate;
};

#endif // SPREADRNG_H
