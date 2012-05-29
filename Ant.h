#ifndef ANT_H_
#define ANT_H_

#include <list>
#include "Task.h"
#include "Move.h"
struct GameState;


enum AntState {WORKING, FIGHTING, WAITING, DEADLOCKED};
/*
    struct for representing my ants 
*/
struct Ant
{
    Location location;
    AntState state;
    Move move;
    Task task;
    std::list<Location> enemies;
    std::list<Location> path; // obj is at end of the path vector
    static double attackradius2, spawnradius2, viewradius2;

    Ant();
    Ant(Location loc);
    //bool readyToMove(int move, GameState &gs);
    int pickMove(GameState &gs);
    int getTaskWeight(const Task &t, GameState &gs) const;
    bool collide( Ant &a, double radius2 );
    void validatePath();
    bool commonEnemy( Ant &a );
    //bool attackCollide( Ant &a );
    //bool viewCollide( Ant &a );
    bool operator==( const Ant &a ) const;
    bool operator!=( const Ant &a ) const;


};

#endif //ANT_H_

