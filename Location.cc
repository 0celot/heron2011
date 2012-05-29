#include "Location.h"
#include <iostream>
#include <cassert>

using namespace std;

const int NO_MOVE = -1;

int Location::numRows = 0;
int Location::numCols = 0;

Location::Location()
{
    row = col = 0;
};

Location::Location(int r, int c)
    : row(r), col(c)
{
};


Location::Location( const Location &l )
    : row(l.row), col(l.col)
{
};

Location::~Location()
{
};

Location &Location::operator=( const Location &l )
{
    if(this == &l)
        return *this;

    row = l.row;
    col = l.col;

    return *this;
};

bool Location::operator==( const Location &l ) const
{
    return (row == l.row && col == l.col);
};

bool Location::operator!=( const Location &l ) const
{
    return !(*this == l);
};

bool Location::operator<( const Location &l ) const
{
    int val = row * numCols + col;
    int val2 = l.row * l.numCols + l.col;

    return (val < val2);
}
    
bool Location::operator>( const Location &l ) const
{
    return !((*this == l) || (*this < l));
};

//returns the euclidean distance between two locations with the edges wrapped
// TODO: consider removing sqrt - bbm
// TODO: make cols and rows from GameState a static variable in Location
double Location::distance(const Location &l)
{
    int d1 = abs(row-l.row),
        d2 = abs(col-l.col),
        dr = min(d1, numRows-d1),
        dc = min(d2, numCols-d2);
    return sqrt(dr*dr + dc*dc);
};

double Location::distance2(const Location &l)
{
    int d1 = abs(row-l.row),
        d2 = abs(col-l.col),
        dr = min(d1, numRows-d1),
        dc = min(d2, numCols-d2);
    return (dr*dr + dc*dc);
};

int Location::manhattanDistance(const Location&l) const
{
    int d1 = (int)abs(row-l.row);
    int d2 = (int)abs(col-l.col);
    int dr = (int)min(d1, numRows-d1);
    int dc = (int)min(d2, numCols-d2);

    return dr + dc;
};

Location Location::absoluteFromRelative( const Location &l ) const
{
    Location a = Location(row + l.row, col + l.col);
    if (a.row >= numRows)
    {
        a.row = a.row % numRows;
    }
    if (a.row < 0)
    {
        a.row = numRows - (((int)abs(a.row)) % numRows );
    }
    if (a.col >= numCols)
    {
        a.col = a.col % numCols;
    }
    if (a.col < 0)
    {
        a.col = numCols - (((int)abs(a.col)) % numCols );
    }

    return a;
};

int Location::adjacentDirection( const Location &l ) const
{

    assert(manhattanDistance(l)<=1);

    int dr1 = (int)abs(row-l.row);
    int dr2 = numRows - dr1;
    int dc1 = (int)abs(col-l.col);
    int dc2 = numCols - dc1;
    
    // 0=N, 1=E, 2=S, 3=W
    if(l.row < row)
    {
        if (dr1 <= dr2)
            return 0;
        else
            return 2;
    }
    else if(l.row > row)
    {
        if (dr1 <= dr2)
            return 2;
        else
            return 0;
    }
    else if(l.col < col)
    {
        if (dc1 <= dc2)
            return 3;
        else
            return 1;
    }
    else if(l.col > col)
    {
        if (dc1 <= dc2)
            return 1;
        else
            return 3;
    }
    else
        return NO_MOVE;
    
}

void Location::direction( std::vector<int> &cdirs, const Location &l) const
{
    int height2 = numRows / 2;
    int width2 = numCols / 2;
    


    // 0=N, 1=E, 2=S, 3=W
    if(row < l.row)
    {
        if(l.row - row >= height2)
            cdirs.push_back(0);
        if(l.row - row <= height2)
            cdirs.push_back(2);
    }
    if(l.row < row)
    {
        if(row - l.row >= height2)
            cdirs.push_back(2);
        if(row - l.row <= height2)
            cdirs.push_back(0);
    }
    if(col < l.col)
    {
        if(l.col - col >= width2)
            cdirs.push_back(3);
        if(l.col - col <= width2)
            cdirs.push_back(1);
    }
    if(l.col < col)
    {
        if(col - l.col >= width2)
            cdirs.push_back(1);
        if(col - l.col <= width2)
            cdirs.push_back(3);
    }
/*
    // Make sure to close distance along longest axis first
        if(cdirs.size()>1)
        {
            int d1 = (int)abs(row-l.row);
            int d2 = (int)abs(col-l.col);
            int dr = (int)min(d1, numRows-d1);
            int dc = (int)min(d2, numRows-d2);

            if(dr > dc)
            {
                if(cdirs[0] == (1 || 3))
                {
                    int temp = cdirs[0];
                    cdirs[0] = cdirs[1];
                    cdirs[1] = temp;
                }
            }
            else if(dr < dc)
            {
                if(cdirs[0] == (0 || 2))
                {
                    int temp = cdirs[0];
                    cdirs[0] = cdirs[1];
                    cdirs[1] = temp;
                }
            }

        }
*/
        
}; 




ostream& operator<<(std::ostream &os, const Location &l)
{
    os << l.row << " " << l.col;
    return os;
};
