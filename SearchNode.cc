#include "SearchNode.h"

SearchNode::SearchNode()
{
}

SearchNode::SearchNode( const Location &l )
    : location(l)
{
}

SearchNode::SearchNode( const SearchNode &n )
    : hval(n.hval), gval(n.gval), fval(n.fval), location(n.location), 
      parentLocation(n.parentLocation)
{
} 

SearchNode::~SearchNode( )
{
}

SearchNode &SearchNode::operator=( const SearchNode &n )
{
    if(this == &n)
        return *this;

    hval = n.hval;
    gval = n.gval;
    fval = n.fval;
    location = n.location;
    parentLocation = n.parentLocation;

    return *this;
}

bool SearchNode::operator<( const SearchNode &n ) const
{
    return fval < n.fval;
}

bool SearchNode::operator>( const SearchNode &n ) const
{
    return fval > n.fval;
}

bool SearchNode::operator==( const SearchNode &n ) const
{
    return location == n.location;
}

bool SearchNode::operator!=( const SearchNode &n ) const
{
    return location != n.location;
}


