#include "spreadrng.h"
#include <random>
#include <QDebug>
#include <ctime>

SpreadRNG::SpreadRNG(int lower, int upper)
    :lower(std::min(lower,upper)), upper(std::max(lower,upper)), rate(new std::vector<double>(upper-lower+1,1))
{
    generator = new std::default_random_engine{static_cast<long unsigned int>(time(0))};
}

void SpreadRNG::reset() {
    rate->clear();
    rate->shrink_to_fit();
    delete rate;
    rate = new std::vector<double>(upper-lower+1,upper-lower);
}

void SpreadRNG::reset(int lower, int upper) {
    this->lower = std::min(lower,upper);
    this->upper = std::max(lower,upper);
    rate->clear();
    rate->shrink_to_fit();
    delete rate;
    rate = new std::vector<double>(upper-lower+1,upper-lower);
}

int SpreadRNG::generate() {
    //generate
    std::uniform_int_distribution<long long unsigned> distribution(0,(upper-lower+1)*1000);
    double r=(double)distribution(*generator)/1000;
    int ans=-1;
    for(int i=0; i<(int)rate->size(); ++i) {
        r-=rate->at(i);
        if(r<0) {
            ans = i;
            break;
        }
    }
    if(ans==-1) ans=rate->size()-1;
    //change rate
    for(double& ra: *rate) ra+=rate->at(ans)/(rate->size()-1);
    rate->at(ans)=0;
    //
    return ans+lower;
}
