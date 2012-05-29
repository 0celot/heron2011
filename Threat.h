#ifndef THREAT_H_
#define THREAT_H_

#include "Location.h"

struct Threat
{
    Location location;
    Location origin;
    Location key;

    Threat( );
    Threat( Location l, Location o, Location k );
    Threat( const Threat &t );
    Threat &operator=( const Threat &t );
    ~Threat();
    friend ostream& operator<<( ostream &os, const Threat &t );
    
    bool operator==( const Threat &t ) const;
    bool operator!=( const Threat &t ) const;

};

#endif //THREAT_H_
