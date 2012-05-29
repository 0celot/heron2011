#ifndef TABU_H_
#define TABU_H_

#include <list>
#include <vector>
#include <algorithm>
#include "GameState.h"
#include "Location.h"
#include "SearchNode.h"

struct SearchMove
{
    int ant;
    int direction;
};


struct Tabu
{

    vector<int> bestSolutio;
    vector<int> currentSolution;

    list<SearchMove> tabuList;
    
/*
    int visited;
    int memberships;
    int inserts;
    int removes;
    int openAdjusts;
    int closedAdjusts;


    std::vector<std::vector<bool> > inOpen;  // indexed array for A*
    std::vector<std::vector<bool> > inClosed;  // indexed array for A*

    std::vector<SearchNode> open;
    std::vector<SearchNode> closed;
*/
    Tabu();
    //Tabu( int rows, int cols );
    ~Tabu();
/*
    void run( std::list<Location> &path, Location start, Location finish, GameState &gs );
    void findPaths( GameState &gs );
    void reset( );

    SearchNode getBestOpen( );
    SearchNode getBestClosed( );
    void addOpen( const SearchNode &c );
    void addClosed( const SearchNode &c );

    bool isInOpen( const SearchNode &n );
    bool isInClosed( const SearchNode &n );
    vector<SearchNode>::iterator findInOpen( const SearchNode &n );
    vector<SearchNode>::iterator findInClosed( const SearchNode &n );
*/
};



#endif //TABU_H_

