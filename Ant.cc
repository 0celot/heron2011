#include <iostream>
#include <algorithm>
#include <vector>

#include "Utility.h"
#include "Location.h"
#include "Ant.h"
#include "GameState.h"


const int SCATTER = 3;

using namespace std;


double Ant::attackradius2 = 0;
double Ant::spawnradius2 = 0;
double Ant::viewradius2 = 0;

Ant::Ant()
{
    location = Location();
    state = WORKING;
    Move move = Move();
    Task task = Task();

};
    
Ant::Ant(Location loc)
    : location(loc)
{
    state = WORKING;   
};
/*
bool Ant::readyToMove(int move, GameState &gs)
{
    Location moveLoc = gs.getLocation(location, move);

    return true;
}
*/

int Ant::pickMove(GameState &gs)
{
    int move = -1;

    vector<int>directions;
    Location nextMove;

    // If we didn't calculate a path then just move straight to the task destination
    if (path.empty()==true)
        nextMove = task.destination;
    // Otherwise move along the path
    else
    {
        int psize = path.size();
    
        // if path size > 1 then get the second move position 
        // b/c this will give us 1-2 move options to get there.
        if(psize > 1)
        {
            list<Location>::iterator pIt = path.begin();
            pIt++;
            nextMove = *pIt; 
        }
        else
            nextMove = *path.begin();
    }
/*
    // Check every adjacent space for friendly ants and/or threats
    vector<bool> occupied = vector<bool>(4,false);
    int numOccupied = 0;
    vector<bool> threatened = vector<bool>(4,false);
    int numThreatened = 0;

    for (int i=0; i<4;++i)
    {
        Location mv = location.direction(i,nextMove);

        if (gs.grid[mv.row][mv.col].ant==0)
        {
            occupied[i] = true;
            ++numOccupied;
        }
        else if (gs.grid[mv.row][mv.col].isHill==true && gs.grid[mv.row][mv.col].hillPlayer==0)
        {
            occupied[i] = true;
            ++numOccupied;
        }
        else if (gs.grid[mv.row][mv.col].ant==0)
        {
            occupied[i] = true;
            ++numOccupied;
        }
    
        int threatCount = 0;
        
        
    }
*/

    location.direction(directions, nextMove);
        
    for (int i=0; i<(int)directions.size(); ++i)
    {
        int d = directions[i]; 
        Location dest = gs.getLocation(location, d);

        // Stop if about to move into food.
        if (gs.grid[dest.row][dest.col].isFood != 0)
        {
            break;
        }
        
        // TODO: This needs to be fixed because right now sometimes a hill will permanently block an ant.
        if ((!gs.grid[dest.row][dest.col].isWater) &&
           (gs.grid[dest.row][dest.col].ant < 0))
        {

            // Test for friendly hill
            if (gs.grid[dest.row][dest.col].isHill==true && gs.grid[dest.row][dest.col].hillPlayer==0)
                continue;

            // Test for friendly ant and move around if necessary
            if (gs.grid[dest.row][dest.col].ant==0)
            {

            }
            
            move = d;
            if (path.empty()==false)
                path.pop_front();
            break;
/*            
            // Move around the hill
            else
            {
                if (directions.size()==1)
                { 
                    int left = (d + 3) % 4;
                    //int back = (d + 2) % 4;
                    int right = (d + 1) % 4;
    
                    int opendirections = 0;
                     
                    double choice = gs.utility.uniformDouble();
                }
            }
*/
        }
    }
    return move;
};

//!
//! Evaluate the weight of a given task relative to the ant. 
//! This considers task description, manhattan distance, game state,
//! and ant state. Lower is better.
//!
int Ant::getTaskWeight(const Task &t, GameState &gs) const
{
    int d_t = location.manhattanDistance(t.destination);

    int w_t = 0;
    // Prefer only 1 ant per food
    if (t.description == GATHER)
    {
        w_t = t.priority + d_t + t.numAnts * 1000;
    }
    else if (t.description == INTERCEPT)
    {
        int d_hill1 = location.manhattanDistance(t.hill);
        int d_hill2 = t.destination.manhattanDistance(t.hill);
        w_t = t.priority +  max(0, d_t + (d_hill1 - d_hill2)) + t.numAnts * SCATTER;
    }
    else if (t.description == PATROL)
    {
        w_t = t.priority + d_t + t.numAnts * 1000;
    }
    else
        w_t = t.priority + d_t + t.numAnts * SCATTER;

    return w_t;
};

bool Ant::commonEnemy( Ant &a ) 
{
    //std::list<Location> common;

    //set_intersection(enemies.begin(), enemies.end(), a.enemies.begin(), a.enemies.end(), common.begin());

    //if (common.empty())
    //    return false;
    //else
    //    return true;

    list<Location>::iterator it1;
    list<Location>::iterator it2;

    for (it1 = enemies.begin(); it1 != enemies.end(); ++it1)
    {
        for (it2 = a.enemies.begin(); it2 != a.enemies.end(); ++it2)
        {
            if (*it1 == *it2)
                return true;
        }
    }

    return false;
};

bool Ant::operator==( const Ant &a ) const
{
    return location == a.location;
};

bool Ant::operator!=( const Ant &a ) const
{
    return !(location == a.location);
};

bool Ant::collide( Ant &a, double radius2 )
{

    if (location.distance2(a.location) < radius2)
        return true;
    else
        return false;
};

void Ant::validatePath()
{
    if (move.onPath==false)
        path.clear();
    else if (!path.empty())
        path.pop_front();
};

/*
bool Ant::spawnCollide( Ant &a )
{
    if (location.distance2(a.location) < (2 * spawnradius2))
        return true;
    else
        return false;
};

bool Ant::attackCollide( Ant &a )
{    
    if (location.distance2(a.location) < (2 * attackradius2))
        return true;
    else
        return false;
};

bool Ant::viewCollide( Ant &a )
{
    if (location.distance2(a.location) < (2 * viewradius2))
        return true;
    else
        return false;
};
*/
