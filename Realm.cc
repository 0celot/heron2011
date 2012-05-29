#include <cassert>
#include <algorithm>
#include "GameState.h"
#include "Realm.h"
#include "Location.h"
#include "Ant.h"


Realm::Realm()
{
}

Realm::Realm( Location hill, int rows, int cols )
    : numRows(rows), numCols(cols)
{
     interior = vector<vector<bool> >(rows, vector<bool>(cols, false));
     accessible = vector<vector<bool> >(rows, vector<bool>(cols, false));

     interior[hill.row][hill.col] = true;
     accessible[hill.row][hill.col] = true;
     frontier.push_back(hill);
}

//! 
//! Test if two realms intersect in at least one interior position.
//! We don't consider accessible spaces being part of the realm unless
//! they are alos in the interior because they may be uncharted.
//!
bool Realm::intersect( const Realm &r )
{
    assert(numRows == r.numRows);
    assert(numCols == r.numCols);

    for (int i=0; i<numRows; ++i)
    {
        for (int j=0; j<numCols; ++j)
        {
            if (interior[i][j] == true && r.interior[i][j] == true)
                return true;
        }
    }

    return false;
}

//!
//! Combine two realms
//!
void Realm::unify( Realm &r )
{
    assert(numRows == r.numRows);
    assert(numCols == r.numCols);

    frontier.splice(frontier.begin(), r.frontier);
    tasks.splice(tasks.begin(), r.tasks);
    antLocations.splice(antLocations.begin(), r.antLocations);

    for (int i=0; i<numRows; ++i)
    {
        for (int j=0; j<numCols; ++j)
        {
            accessible[i][j] == accessible[i][j] || r.accessible[i][j];
            interior[i][j] == interior[i][j] || r.interior[i][j];
        }
    } 
}

void Realm::filterTasks( list<Task> &ts, GameState &gs )
{

    list<Task>::iterator tIt = ts.begin();

    while(tIt != ts.end())
    {
        Location d = tIt->destination;
        if( accessible[d.row][d.col] == true )
        {
            //gs.bug << "accessible task: " << tIt->destination << std:: endl;
            tasks.push_back(*tIt); 
        }
        ++tIt;
    } 
}

//!
//! Check to make sure that ants in the realm surviving from the
//! previous turn still have valid tasks.
//!
void Realm::filterSurvivingAntsTasks( GameState &gs )
{
    // Validate tasks for surviving ants
    for (int i=0; i<(int)gs.myAnts.size(); ++i)
    {    
        Location antLoc = gs.myAnts[i].location;

        if ( interior[antLoc.row][antLoc.col] == true )
        { 
            bool validtask = false;
            list<Task>::iterator tIt = tasks.begin();
            while (tIt != tasks.end())
            {
                if(tIt->destination==gs.myAnts[i].task.destination)
                {
                    validtask = true;
                    // This task assignment is still in use
                    tIt->numAnts++;
                    break;
                }
                ++tIt;
            }

            // surviving ants with invalid tasks must get new tasks
            if(validtask==false)
            {
                gs.myAnts[i].task.description = NONE;
            }

        }

    }
}

void Realm::assignTasks( GameState &gs )
{
    // Don't assign tasks if there aren't any to assign.
    if(tasks.size()==0)
    {
        return;
    }

    for (int i=0; i<(int)gs.myAnts.size(); ++i)
    {
        Location antLoc = gs.myAnts[i].location;

        // Only assign tasks to ants in this realm
        if ( interior[antLoc.row][antLoc.col] == true )
        { 
            Task bestTask;
            list<Task>::iterator tIt = tasks.begin();
            Task *newTaskPtr = 0;

            // Ant without a task?
            if (gs.myAnts[i].task.description == NONE)
            {
                bestTask = *tIt; 
                newTaskPtr = &(*tIt);
                ++tIt;
            }
            else
            {
                bestTask = gs.myAnts[i].task;
            }
            
            int w_bestTask = gs.myAnts[i].getTaskWeight(bestTask, gs);
    
            // Find the best task out of all available tasks 
            while (tIt != tasks.end())
            {
                Task t = *tIt;
                int w_t = gs.myAnts[i].getTaskWeight(t, gs);

                if (w_bestTask > w_t)
                {
                    bestTask = t;   
                    w_bestTask = w_t;
                    newTaskPtr = &(*tIt);

                }

                ++tIt;
            } 
                
            // Update with new best task if neccessary
            if(gs.myAnts[i].task != bestTask)
            {
                // If an old task was replaced update the number of ants for that task
                tIt = find(tasks.begin(), tasks.end(), gs.myAnts[i].task);
                if (tIt!=tasks.end())
                    tIt->numAnts--;

                //int dist = gs.myAnts[i].task.destination.manhattanDistance(bestTask.destination); 

                // In case of moving enemy ants or frontier exploration make sure to 
                // preserve a valid path
//                if (dist==1)
//                {
/*
                    gs.bug << "dist is one from dest " << gs.myAnts[i].task.destination <<
                        " to dest " << bestTask.destination << std::endl;

                    gs.bug << "path before push back" << std::endl;
                    list<Location>::iterator pIt = gs.myAnts[i].path.begin();
                    while(pIt != gs.myAnts[i].path.end())
                    {
                        gs.bug << *pIt << " : ";
                        ++pIt;
                    }
                    //for (int j=0;j<(int)gs.myAnts[i].path.size();++j)
                    //    gs.bug << gs.myAnts[i].path[j] << " : ";
                    gs.bug << std::endl;
*/
                    gs.myAnts[i].path.push_back(bestTask.destination);
/*
                    //gs.myAnts[i].path.push_back(bestTask.destination);
                    gs.bug << "path after push back" << std::endl;
                    pIt = gs.myAnts[i].path.begin();
                    while(pIt != gs.myAnts[i].path.end())
                    {
                        gs.bug << *pIt << " : ";
                        ++pIt;
                    }
                    //for (int j=0;j<(int)gs.myAnts[i].path.size();++j)
                    //    gs.bug << gs.myAnts[i].path[j] << " : ";
                    gs.bug << std::endl;
*/
//                }
//                else
                    gs.myAnts[i].path.clear();
                
                gs.myAnts[i].task = bestTask;

                // If we found a new task, update it
                if (newTaskPtr != 0)
                    newTaskPtr->numAnts++;
            }
/*
            if (gs.myAnts[i].task.description==PATROL)
            {
                gs.bug << "PATROL from " << gs.myAnts[i].location << " to " << gs.myAnts[i].task.destination << std::endl;
                //gs.bug << "INTERCEPT from " << gs.myAnts[i].location << " to " << gs.myAnts[i].task.destination << std::endl;
            }
*/
        }
    }

}

void Realm::bfs( GameState &gs )
{

    Location c;
    Location n;
    list<Location> f;

    while (frontier.size() > 0)
    {
        c = frontier.front();
        bool cf = false;
        

        for (int i=0; i<4; ++i)
        {
            n = gs.getLocation(c,i);
           
            // don't care about locations in the interior
            if (interior[n.row][n.col] == true)
                continue;
            
            if (gs.grid[n.row][n.col].state == UNCHARTED)
            {
                accessible[n.row][n.col] = true;
                cf = true;               
                continue;
            }

            if (gs.grid[n.row][n.col].isWater == false)
            {
                accessible[n.row][n.col] = true;
                interior[n.row][n.col] = true;
                frontier.push_back(n);
            }
        }

        if (cf)
        {
            f.push_back(c);
        }

        frontier.pop_front();

    }

    frontier.splice(frontier.end(), f);
}

