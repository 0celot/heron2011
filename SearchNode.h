#ifndef SEARCHNODE_H_
#define SEARCHNODE_H_

//#include <vector>
#include "Location.h"

struct SearchNode
{

    int hval;
    int gval;
    int fval;
    Location location;
    Location parentLocation;
    //SearchNode * parent;

    SearchNode();
    SearchNode( const Location &l );
    SearchNode( const SearchNode &n );
    //SearchNode( Location loc, SearchNode const *par);
    ~SearchNode();
    SearchNode &operator=( const SearchNode &n );
   
    bool operator<( const SearchNode &n ) const; 
    bool operator>( const SearchNode &n ) const; 
    bool operator==( const SearchNode &n ) const;
    bool operator!=( const SearchNode &n ) const;
};






#endif //SEARCHNODE_H_
