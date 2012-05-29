#include "Threat.h"

using namespace std;  

Threat::Threat( )
{
};

Threat::Threat( Location l, Location o, Location k )
    : location(l), origin(o), key(k)
{
};

Threat::Threat( const Threat &t )
    : location(t.location), origin(t.origin), key(t.key)
{
};

Threat &Threat::operator=( const Threat &t )
{
    if(this == &t)
        return *this;

    location = t.location;
    origin = t.origin;
    key = t.key;

    return *this;
};

Threat::~Threat()
{
};

bool Threat::operator==( const Threat &t ) const
{
    return (key == t.key);
};

bool Threat::operator!=( const Threat &t ) const
{
    return !(key == t.key);
};

ostream& operator<<(std::ostream &os, const Threat &t)
{
    os << "("<< t.location <<", " << t.origin << ", " << t.key << ")";
    return os;
};
