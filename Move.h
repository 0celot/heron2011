#ifndef MOVE_H_
#define MOVE_H_

#include <iostream>
#include <vector>
#include <cassert>
#include "Location.h"
#include "Threat.h"

//enum MoveState {UNCHARTED, CHARTED, VISIBLE};


const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};

const int NO_MOVE = -1; 

/*
    struct for representing a square in the grid.
*/
struct Move
{
    Location origin;
    int direction;
    bool consistent;
    bool onPath;
    bool safe;
    bool unthreatened;

    Move()
    {
        origin = Location(0,0);
        direction = NO_MOVE;
        consistent = false;
        onPath = false;
        safe = false;
        unthreatened = false;
    };

    Move( Location o, int dir )
        : origin(o), direction(dir)
    {

    };

    Move( const Move &m )
        : origin(m.origin), direction(m.direction), consistent(m.consistent),
          onPath(m.onPath), safe(m.safe) 
    {
    };

    Move &operator=( const Move &m )
    {
        if(this == &m)
            return *this;

        origin = m.origin;
        direction = m.direction;
        consistent = m.consistent;
        onPath = m.onPath;
        safe = m.safe;

        return *this;
    };

    void reset()
    {
        origin = Location(0,0);
        direction = NO_MOVE;
        consistent = false;
        onPath = false;
        safe = false;
        unthreatened = false;
    };

    ~Move()
    {
    };

    // This means worse than and not neccessarily less than
    bool operator<( const Move &m ) const
    {
        int val = 0;

        if (!onPath)
            val = val + 1;
        if (!unthreatened)
            val = val + 2;
        if (!safe)
            val = val + 4;

        int val2 = 0;

        if (!m.onPath)
            val2 = val2 + 1;
        if (!m.unthreatened)
            val2 = val2 + 2;
        if (!m.safe)
            val2 = val2 + 4;

        // Higher numbers are bad and so they need
        // to be first in the list
        return (val < val2); 
    }


    friend ostream& operator<<( ostream &os, const Move &m )
    {
        assert(m.direction >= NO_MOVE && m.direction < 4);

        if (m.direction != NO_MOVE)
            os << m.origin.row << " " << m.origin.col << " " << CDIRECTIONS[m.direction];

        return os; 
    };

};

#endif //MOVE_H_

