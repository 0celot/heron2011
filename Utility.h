#ifndef UTILITY_H_
#define UTILITY_H_

#include <cmath>
#include <iostream>
//#include <cstdlib>
#include <vector>

using namespace std;
/*
    struct for representing locations in the grid.
*/
struct Utility
{
    unsigned long r;

    Utility();
    ~Utility();
    unsigned long xorshift();

    // A uniform random deviate from 0 to 1 
    double uniformDouble();

    int geometricDeviate( int n );
};


#endif //UTILITY_H_
