#include "Utility.h"

Utility::Utility()
    :r(88172645463325252)
{
}

Utility::~Utility()
{
}


unsigned long Utility::xorshift()
{
    r^=(r<<13);
    r^=(r>>7);
    r^=(r<<17);
    return r;
};

// A uniform random deviate from 0 to 1 
double Utility::uniformDouble()
{
    return 5.42101086242752217E-20 * xorshift();
}; 

// Not really a geometric deviate - just something close to it
int Utility::geometricDeviate( int n )
{
    double x = uniformDouble();
    double y = sin(x * M_PI);
    double z = abs(y-1);
    
    return (int) floor(z * n);
};
