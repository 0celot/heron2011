#ifndef REALM_H_
#define REALM_H_

#include <vector>
#include <list>
#include "Ant.h"
#include "Task.h"

struct Realm
{
    int numRows;
    int numCols;

    std::list<Task> tasks;
    std::list<Ant> antLocations;
    std::list<Location> frontier; // outermost locations of the interior
    std::vector<std::vector<bool> > interior;  // includes the frontier
    std::vector<std::vector<bool> > accessible; // includes the interior and uncharted edges

    Realm();
    Realm( Location hill, int rows, int cols );
    bool intersect( const Realm &r );
    void unify( Realm &r );
    void filterTasks( list<Task> &ts, GameState &gs );
    void filterSurvivingAntsTasks( GameState &gs );
    void assignTasks( GameState &gs );
    void bfs( GameState &gs );
 
};

#endif //REALM_H_

