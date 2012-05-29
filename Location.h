#ifndef LOCATION_H_
#define LOCATION_H_

#include <cmath>
#include <iostream>
//#include <cstdlib>
#include <vector>

using namespace std;
/*
    struct for representing locations in the grid.
*/
struct Location
{
    static int numRows, numCols;
    int row, col;

    Location();
  
    Location(int r, int c);
       
    Location( const Location &t );

    ~Location();

    Location &operator=( const Location &l );


    bool operator==( const Location &l ) const;
    bool operator!=( const Location &l ) const;
    bool operator<( const Location &l ) const;
    bool operator>( const Location &l ) const; 
   
    //returns the euclidean distance between two locations with the edges wrapped
    double distance( const Location &l );
    double distance2( const Location &l );
    Location absoluteFromRelative( const Location &l ) const;
    int manhattanDistance( const Location&l ) const; 
    int adjacentDirection( const Location &nextLocation ) const;
    void direction( vector<int> &cdirs, const Location &l ) const;
    friend ostream& operator<<( ostream &os, const Location &l );
 

};


#endif //LOCATION_H_
