#include <cassert>
#include <algorithm>
#include <vector>
#include <list>
#include <climits>
#include <cmath>

#include "GameState.h"
#include "Bot.h"
#include "Task.h"
#include "Utility.h"

using namespace std;

// Charted squares become patrol tasks after this many turns
const int REDISCOVERY_THRESHOLD = 20;
const int TABU_LIST_LENGTH = 10;

Bot::Bot()
{

};


Bot::~Bot()
{
}

void Bot::generateTasks( list<Task> &tasks, GameState &gs )
{
    //gs.bug << "generateTasks " << std::endl;
    for (int i=0; i<(int)gs.enemyHills.size(); ++i)
    {
        //gs.bug << "enemy hill task at " << gs.enemyHills[i] << std::endl;
        tasks.push_back(Task(gs.enemyHills[i], RAZE));
    }
    
    for (int i=0; i<(int)gs.food.size(); ++i)
    {
        //gs.bug << "food task at " << gs.food[i] << std::endl;
        tasks.push_back(Task(gs.food[i], GATHER));
    }

    //gs.bug << "viewradius2: " << Ant::viewradius2 << std::endl;
    for (int i=0; i<(int)gs.enemyAnts.size(); ++i)
    {
        for (int j=0; j<(int)gs.myHills.size(); ++j)
        {
            double dist = gs.enemyAnts[i].distance2(gs.myHills[j]);
            //gs.bug << "Distance to Hill: " << dist << std::endl;
            if (dist<=Ant::viewradius2)
            {
                tasks.push_back(Task(gs.myHills[j], gs.enemyAnts[i], INTERCEPT));
                //gs.bug << "Ant for INTERCEPT at " << gs.enemyAnts[i] << std::endl;
            }
        }
    }

    int tempTotal = 0;
    int tempUncharted = 0;

    for (int i=0; i<gs.rows; ++i)
    {
        for (int j=0; j<gs.cols; ++j)
        {
            tempTotal++; 
            // Test adjacent squares
            if (gs.grid[i][j].state == UNCHARTED)
            {
                tempUncharted++;
                int i_plus1 = (i + 1) % (gs.rows);
                int j_plus1 = (j + 1) % (gs.cols);
                int i_minus1 = i-1;
                if(i_minus1 < 0) 
                    i_minus1 += gs.rows;
                int j_minus1 = j-1;
                if(j_minus1 < 0) 
                    j_minus1 += gs.cols;
        
                assert(i_plus1 >= 0 && i_plus1 < gs.rows);
                assert(i_minus1 >= 0 && i_minus1 < gs.rows);
                assert(i >= 0 && i < gs.rows);
                assert(j_plus1 >= 0 && j_plus1 < gs.cols);
                assert(j_minus1 >= 0 && j_minus1 < gs.cols);
                assert(j >= 0 && j < gs.cols);
                // Tests true for uncharted squares adjacent to either charted or visible squares
                if ((gs.grid[i_plus1][j].state == VISIBLE) || 
                    (gs.grid[i_plus1][j].state == CHARTED) ||
                    (gs.grid[i][j_plus1].state == VISIBLE) || 
                    (gs.grid[i][j_plus1].state == CHARTED) ||
                    (gs.grid[i_minus1][j].state == VISIBLE) ||
                    (gs.grid[i_minus1][j].state == CHARTED) ||
                    (gs.grid[i][j_minus1].state == VISIBLE) ||
                    (gs.grid[i][j_minus1].state == CHARTED))
                {
                    //gs.bug << "uncharted at " << i << ", " << j << std::endl;
                    tasks.push_back(Task(Location(i,j), EXPLORE)); 
                }

            }

            else if (gs.grid[i][j].state == CHARTED)
            {
                if (gs.grid[i][j].counter > REDISCOVERY_THRESHOLD)
                {
                    Task patrolTask = Task(Location(i,j), PATROL);
                    tasks.push_back(patrolTask);

                    for (int k=0; k<(int)gs.viewRange.size(); ++k)
                    {
                        Location resetLoc = patrolTask.destination.absoluteFromRelative(gs.viewRange[k]);
                        gs.grid[resetLoc.row][resetLoc.col].resetCounter();
                    }
                }
            }

        }
    }
}



void Bot::updateRealms( list<Realm> &r, GameState &gs)
{

    // Update interior and accessible regions
    list<Realm>::iterator it1 = r.begin();
    while (it1 != r.end())
    {
        it1->bfs(gs);
        //bfs(*it1, gs);
        ++it1;       
    }

    // Unify intersecting realms
    it1 = r.begin();
    list<Realm>::iterator it1stop = r.end();
    --it1stop;
    while (!((it1 == it1stop) || (it1 == r.end())))
    {
        list<Realm>::iterator it2 = it1;
        ++it2; 
        while (it2 != r.end())
        {
            if (it1->intersect(*it2))
            {
                it1->unify(*it2);
                it2 = r.erase(it2);
            }
            else
            {
                ++it2;
            }
        }

        ++it1;
    }

}

list<int> Bot::bfs( vector<vector<bool> > &graph, int start )
{
    list<int> searchQueue;
    searchQueue.push_back(start);
    
    list<int> antGroup;
    antGroup.push_back(start);

    
    while (!searchQueue.empty())
    {
        int parent = searchQueue.front();
        searchQueue.pop_front();

        //gameState.bug << "examining parent " << parent << std::endl;
        //gameState.bug << graph[parent][parent] << std::endl;
        
        for (int i=0; i<(int)graph.size(); ++i)
        {
            //gameState.bug << "examining neighbor " << i << std::endl;
            //gameState.bug << graph[parent][i] << std::endl;
            if (parent != i)
            {
                //gameState.bug << "not the same" << std::endl;
                //gameState.bug << graph[parent][i] << std::endl;
                if (graph[parent][i]==true)
                {
                    //gameState.bug << "true node: " << i << std::endl;
                    list<int>::iterator aIt = find(antGroup.begin(), antGroup.end(), i);
                    if (aIt == antGroup.end())
                    {
                        searchQueue.push_back(i);
                        antGroup.push_back(i);
                    }
                    //else
                    //{
                        //gameState.bug << "antGroup has " << *aIt << std::endl;
                    //}
                }
            }
        }
    }

    return antGroup;
};

bool compareAntGroups( list<int> &a, list<int> &b)
{
    if ( a.size() <= b.size() )
        return true;
    else
        return false;  
};

//plays a single game of Ants.
void Bot::playGame()
{
    //reads the game parameters and sets up
    cin >> gameState;
    gameState.setup();
    endTurn();

    //continues making moves while the game is not over
    while(cin >> gameState)
    {

        // First turn initialization
        if (gameState.turn==1)
        {
            // Create test ranges
            gameState.makeRange(gameState.viewRange, gameState.viewradius2); 
            gameState.makeRange(gameState.attackRange, gameState.attackradius2); 
            gameState.makeRange(gameState.spawnRange, gameState.spawnradius2);
            gameState.threatRange = gameState.makeThreatRange(gameState.attackRange); 
    
            // Create realms
            for (int i=0; i<(int)gameState.myHills.size(); ++i)
            {
                Realm r(gameState.myHills[i], gameState.rows, gameState.cols);
                realms.push_back(r); 
            }

            // Initialize pathfinding
            pathFinder = AStar(gameState.rows, gameState.cols);
            pathFinder.reset();
        }

        // Update the game state
        gameState.updateVisionInformation();
        gameState.updateFoodInformation();
        gameState.updateHillsInformation();
        gameState.updateThreatInformation();
        gameState.updateEnemyInformation();

        //gameState.printThreats();



        //gameState.bug << "calling makeMoves() " << std::endl;
        makeMoves();
        //gameState.bug << "calling endTurn() " << std::endl;
        endTurn();
    }
};

void Bot::makeAntCollisionMatrix ( vector<vector<bool> > &mat, GameState &gs )
{
    // Adjacency matrix of ant collisions
    //double collisiondist2 = sqrt(gs.attackradius2);
    //collisiondist2 = 2 * collisiondist2;
    //collisiondist2 = collisiondist2 * collisiondist2;
    //gs.findAntCollisions(mat, collisiondist2);
    
    // detect any ant within euclidian distance of 2
    double collisiondist2 = 4.1;
    gs.findAntCollisions(mat, collisiondist2);

};

// Ant groups are graphs of ants that are within the collision radii of each other
void Bot::makeAntGroups( vector<vector<bool> > &collisions, list<list<int> > &groups )
{
    list<int> processed;
    for (int i=0; i<(int)collisions.size(); ++i)
    {
        // Only consider the ant if it isn't currently in a group
        list<int>::iterator pIt = find(processed.begin(), processed.end(), i);
        if (pIt==processed.end())
        {
            list<int> group = bfs(collisions, i);
            processed.insert(processed.end(), group.begin(), group.end());
            groups.push_back(group); 
        }
    }
    
    // Sort the ant groups according to size (largest group last)
    groups.sort(compareAntGroups); 

};

// Combat groups are collections of ant groups that are part of bipartite
// graphs including enemy ants
void Bot::makeCombatGroups( list<list<int> > &groups, GameState &gs )
{
    list<list<int> > combatGroups;
    list<list<int> >::iterator agIt;

    list<int> currentGroup = groups.front();
    groups.pop_front();
    
    while (!groups.empty())
    {
        bool common = false;

        for (agIt = groups.begin(); agIt != groups.end(); ++agIt)
        {
            if (commonEnemy(currentGroup, *agIt, gs))   
            {
                //gs.bug << "commonEnemy" << std::endl;
                agIt->splice(agIt->begin(), currentGroup);
                agIt->sort();

                common = true; 
                break;
            }
        }

        if (!common)
        {
            combatGroups.push_back(currentGroup);
        }
        
        currentGroup = groups.front();
        groups.pop_front();
    } 

    combatGroups.push_back(currentGroup);

    groups = combatGroups;
};


bool Bot::commonEnemy( list<int> &antGroup1, list<int> &antGroup2, GameState &gs )
{
    list<int>::iterator g1;
    for (g1 = antGroup1.begin(); g1 != antGroup1.end(); ++g1)
    {
        // Don't bother matching if no enemies in group 1
        if (gs.myAnts[*g1].enemies.empty())
            continue;

        //gs.bug << "ant " << gs.myAnts[*g1].location << " numenemies: " << gs.myAnts[*g1].enemies.size() << std::endl;
        
        //list<Location>::iterator eIt;
        //for (eIt = gs.myAnts[*g1].enemies.begin(); eIt != gs.myAnts[*g1].enemies.end(); ++eIt)
        //{
        //    gs.bug << "enemy location: " << *eIt << std::endl;
        //}


        list<int>::iterator g2;
        for (g2 = antGroup2.begin(); g2 != antGroup2.end(); ++g2)
        {
            if (gs.myAnts[*g2].enemies.empty())
                continue;

            //gs.bug << "group2 ant " << gs.myAnts[*g2].location << " numenemies: " << gs.myAnts[*g2].enemies.size() << std::endl;
            
            //list<Location>::iterator eIt;
            //for (eIt = gs.myAnts[*g2].enemies.begin(); eIt != gs.myAnts[*g2].enemies.end(); ++eIt)
            //{
            //    gs.bug << "enemy location: " << *eIt << std::endl;
            //}

            if (gs.myAnts[*g1].commonEnemy(gs.myAnts[*g2]))
                return true;
        }
    }

    return false;
}

/// Returns true is the ant's move is consistent and false otherwise
/// i is the ith ant in the antGroup list
// ant is an index into gs.myAnts
// antGroup has indexes into gs.myAnts
bool Bot::isConsistent( list<int> &antGroup, int ant, GameState &gs )
{
    vector<Location> antLocations = vector<Location>(antGroup.size(), Location());
    vector<Location> moveLocations = vector<Location>(antGroup.size(), Location());

    
    int grpIndex = 0;
    int i = 0;
    list<int>::iterator aIt;
    for (aIt = antGroup.begin(); aIt != antGroup.end(); ++aIt)
    {
        if (*aIt==ant)
        {
            grpIndex = i;
        }
        antLocations[i] = gs.myAnts[*aIt].location;
        moveLocations[i] = gs.getLocation(antLocations[i], gs.myAnts[*aIt].move.direction);
        ++i;
    }

    // Is there water or a friendly hill at the move location?
    Location moveloc = moveLocations[grpIndex];
    Location antloc = antLocations[grpIndex];
    Square movesquare = gs.grid[moveloc.row][moveloc.col];

    if (movesquare.isWater)
        return false;

    if (movesquare.isHill && (movesquare.hillPlayer == 0))
        return false;

    if (movesquare.isFood)
        return false; 

    // Is another ant moving to the move location?
    int j = 0;
    for (j=0; j<(int)moveLocations.size(); ++j)
    {
        if (moveloc == moveLocations[j] && antloc != antLocations[j])
        {
            return false;
        }
    }


    // Is there another ant at the move location?
    bool occupied = false;
    for (i=0; i<(int)antLocations.size(); ++i)
    {
        // Another ant is here
        if (moveloc == antLocations[i] && moveloc != antloc)
        {
            occupied = true;
            break;
        }
    }  


    if (occupied == false)
    {
        return true;
    }
    // Check for a cycle
    else
    {
        list<Location> cycle;
        cycle.push_back(antloc); 
        while (occupied)
        {
            occupied = false;
            Location otherAntMove = moveLocations[i];

            // This is a cycle
            list<Location>::iterator cIt = find(cycle.begin(), cycle.end(), otherAntMove);
            if (cIt != cycle.end())
            {
                return false;
            }
            cycle.push_back(otherAntMove);

            // Check the move location for another ant
            for (j=0; j<(int)antLocations.size(); ++j)
            {
                if (otherAntMove==antLocations[j])
                {
                    occupied = true;
                    otherAntMove = moveLocations[j];
                    i = j;
                    break;
                }
            }

        }

        // Not a cycle
        return true;

    }
    
    return true;
}

// Clear paths of any ants that are no longer on their paths
void Bot::validatePaths( list<int> &antGroup, GameState &gs )
{
    list<int>::iterator aIt;
    for (aIt = antGroup.begin(); aIt != antGroup.end(); ++aIt)
    {
        gs.myAnts[*aIt].validatePath();
    }
    
}

void Bot::makeConsistent( list<int> &antGroup, GameState &gs )
{

    // Initialize groupMoves to either the first path square or NO_MOVE if there is no path   
    list<int>::iterator aIt;
    for (aIt = antGroup.begin(); aIt != antGroup.end(); ++aIt)
    {
        int ant = *aIt;
        gs.myAnts[ant].move.origin = gs.myAnts[ant].location;

        if (gs.myAnts[ant].path.empty() == true)
            gs.myAnts[ant].move.direction = NO_MOVE;
        else
        {
            Location nextLoc = gs.myAnts[ant].path.front();
            //gs.bug << "makeConsistent - currentLoc: " << gs.myAnts[ant].location << " nextLoc: " << nextLoc << std::endl;
            gs.myAnts[ant].move.direction = gs.myAnts[ant].location.adjacentDirection(nextLoc);
            gs.myAnts[ant].move.onPath = true;
        }

    } 

    // Make sure each move is consistent
    for (aIt = antGroup.begin(); aIt != antGroup.end(); ++aIt)
    {
        int ant = *aIt;
        bool consistent = isConsistent(antGroup, ant, gs);

        if(consistent == true)
        {
            continue;
        }
         
        // Select an alternate move 
        gs.myAnts[ant].move.onPath = false;
        for (int j=0; j<3; ++j)
        {
            int move = gs.myAnts[ant].move.direction;
            
            move = (move + 1) % 4;

            gs.myAnts[ant].move.direction = move;
            consistent = isConsistent(antGroup, ant, gs);
            
            if (consistent == true)
            {
                break;   
            } 
        }

        if (consistent == false)
            gs.myAnts[ant].move.direction = NO_MOVE;

    }

}

void Bot::makeSafe( list<int> &antGroup, GameState &gs )
{
    gs.bug << "makeSafe 1" << std::endl;
    list<int>::iterator aIt;
    for (aIt = antGroup.begin(); aIt != antGroup.end(); ++aIt)
    {
        Move m = gs.myAnts[*aIt].move;
        Location dest = gs.getLocation(m.origin, m.direction);

        if (gs.grid[dest.row][dest.col].threats.empty())
        {
            continue;
        }
        else
        {
            gs.bug << "ant: " << m.origin << " with move to " << dest << " has " << gs.grid[dest.row][dest.col].threats.size() << " threats and is unsafe." << std::endl;
        }

        Move m2 = m;
        int tries = 0;
        //bool safe = false;
        bool consistent = false;
        
        while(tries < 4)
        {
            ++tries;
            m2.direction = m.direction + tries;
            //m2.direction = -1;
            if (m2.direction > 3)
                m2.direction -= 5; 
            
            //int ant = gs.getAntIndexFromLocation(m2.origin);
            //assert(ant < (int)gs.myAnts.size());
            gs.myAnts[*aIt].move = m2;

            consistent = isConsistent(antGroup, *aIt, gs);
            dest = gs.getLocation(m2.origin, m2.direction);

            if (consistent) 
            {
                gs.myAnts[*aIt].move.consistent = true;
                

                //gs.bug << "Consistent move for ant " << gs.myAnts[ant].location << " to " << dest << std::endl;
                if (gs.myAnts[*aIt].path.front()==dest)
                {
                    //gs.bug << "Ant " << gs.myAnts[ant].location << " move to " << dest << " is on path" << std::endl;
                    gs.myAnts[*aIt].move.onPath = true;
                }
                else
                {
                    gs.myAnts[*aIt].move.onPath = false;
                }

            }
            else // revert to previous move
            {
                gs.myAnts[*aIt].move = m;
                continue;
            }

            if (gs.grid[dest.row][dest.col].threats.empty())
                break;
            else
            {
                gs.myAnts[*aIt].move = m;
                continue;
            }
        }
    }
    
    //gs.bug << "makeSafe 2" << std::endl;
}


void Bot::tabuSearch( list<int> &antGroup, GameState &gs )
{
    double t = gs.timer.getTime();
    if (t > 400)
        return;

    int bestScore = evaluateGroupMoves(antGroup, gs);
    //gs.bug << "tabuSearch - bestScore: " << bestScore << std::endl;
    if (bestScore == 0)
        return;  
 
    bool stop = false;
    int improveCount = 0;

    int stopCondition = max((int)antGroup.size(), 12);
    int maxTabuListLength = min((int)antGroup.size()-1, TABU_LIST_LENGTH);

    list<Move> bestMovesList;

    //int groupsize = antGroup.size();

    list<int>::iterator antItr;
    for (antItr = antGroup.begin(); antItr != antGroup.end(); ++antItr)
    {
        //if (groupsize > 2)
            //gs.bug <<"tabuSearch - ant: " << gs.myAnts[*antItr].location << std::endl;
        Move m = gs.myAnts[*antItr].move;
        m.origin = gs.myAnts[*antItr].location;
        //if (groupsize > 2)
            //gs.bug <<"m safe: " << m.safe << " m.unthreatened: " << m.unthreatened << " m.onPath: " << m.onPath << std::endl;
        bestMovesList.push_back(m);       
    }    

    list<Move> tabuList;
    
    list<Move> currentMovesList = bestMovesList;
    //f

    // sort by fitness
    currentMovesList.sort();

    //Move temp = currentMovesList.front();
    //gs.bug <<"temp move safe: " << temp.safe << " unthreatened: " << temp.unthreatened << " onPath: " << temp.onPath << std::endl;

    while (!stop)
    {

        // Randomly select a move from the moves list to improve
        int idx = gs.utility.geometricDeviate(currentMovesList.size()); 
        Move m;
        int i = 0;
        list<Move>::iterator mvIt;
        for (mvIt = currentMovesList.begin(); mvIt != currentMovesList.end(); ++mvIt)
        {
            if (i==idx)
            {
                m = *mvIt;
                currentMovesList.erase(mvIt);

                break;
            }
            ++i;
        }

        // Try to improve the move
        Move m2;
        m2.origin = m.origin;

        int tries = 0;
        bool consistent = false;
        
        while(tries < 4)
        {
            ++tries;
            m2.direction = m.direction + tries;
            //m2.direction = -1;
            if (m2.direction > 3)
                m2.direction -= 5;

            int ant = gs.getAntIndexFromLocation(m2.origin);
            //gs.bug << "Ant " << gs.myAnts[ant].location << " tries: " << tries << " direction: " << m2.direction << std::endl;;
            //gs.bug << "m2.origin: " << m2.origin << std::endl;
            //gs.bug << "antval: " << ant << std::endl;
            //assert(ant < (int)gs.myAnts.size());
            gs.myAnts[ant].move = m2;

            consistent = isConsistent(antGroup, ant, gs);

            // Set consistent and onPath values for new move.
            // Safe value is set in call to evaluateGroupMoves
            if (consistent) 
            {
                gs.myAnts[ant].move.consistent = true;
                
                Location dest = gs.getLocation(m2.origin, m2.direction);
                //gs.bug << "Consistent move for ant " << gs.myAnts[ant].location << " to " << dest << std::endl;
                if (gs.myAnts[ant].path.front()==dest)
                {
                    //gs.bug << "Ant " << gs.myAnts[ant].location << " move to " << dest << " is on path" << std::endl;
                    gs.myAnts[ant].move.onPath = true;
                }
                else
                {
                    gs.myAnts[ant].move.onPath = false;
                }

                break;  
            }
            else // revert to previous move
            {
                gs.myAnts[ant].move = m;
            }
        }

        tabuList.push_back(m2);
        tabuList.sort();     
  
        // Return a move from the tabu list if neccessary 
        if ((int)tabuList.size() > maxTabuListLength)
        {
            Move temp = tabuList.front();
            tabuList.pop_front();
            currentMovesList.push_back(temp);
        }  


        // Evaluate the new move configuration
        int currentScore = evaluateGroupMoves(antGroup, gs);
        //gs.bug << "tabuSearch() current score: " << currentScore << std::endl; 

        if (currentScore < bestScore)
        {
            bestScore = currentScore;
            gs.bug << "tabuSearch() newbestscore: " << bestScore << std::endl; 

           
            bestMovesList.clear(); 
            list<int>::iterator antItr;
            for (antItr = antGroup.begin(); antItr != antGroup.end(); ++antItr)
            {
                Move m = gs.myAnts[*antItr].move;
                m.origin = gs.myAnts[*antItr].location;
                bestMovesList.push_back(m);       
            }    

            improveCount = 0;
        }
        else
            ++improveCount;

        if (bestScore == 0)
            stop = true;
    
        if (improveCount >= stopCondition)
            stop = true;

        t = gs.timer.getTime();
        if (t > 400)
            stop = true;

    }

    // record the new best move for each ant    
    assert(antGroup.size() == bestMovesList.size());
    list<Move>::iterator bestMovesItr = bestMovesList.begin();
    for (antItr = antGroup.begin(); antItr != antGroup.end(); ++antItr)
    {
        gs.myAnts[*antItr].move = *bestMovesItr;
        ++bestMovesItr;
    }

}




int Bot::evaluateGroupMoves( list<int> &antGroup, GameState &gs )
{
    //gs.bug << "evaluateGroupMoves() 1 " << std::endl;

    vector<Location> moveLocations = vector<Location>(antGroup.size(), Location());
    
    int i = 0;
    list<int>::iterator aIt;
    for (aIt = antGroup.begin(); aIt != antGroup.end(); ++aIt)
    {
        moveLocations[i] = gs.getLocation(gs.myAnts[*aIt].location, gs.myAnts[*aIt].move.direction);
        //moveLocations[i] = gs.getLocation(gs.myAnts[*aIt].location, groupMoves[i]);
        //gs.bug << "move location: " << moveLocations[i] << std::endl;
        ++i;
    }


    vector<int> moveLocOutDegree = vector<int>(antGroup.size(), 0);
    for (i=0; i<(int)moveLocOutDegree.size(); ++i)
    {
        Location moveLoc = moveLocations[i];
        moveLocOutDegree[i] = gs.grid[moveLoc.row][moveLoc.col].threatTotal;

        for(int j=0; j<(int)gs.grid[moveLoc.row][moveLoc.col].threats.size(); ++j)
        {
            Location origin = gs.grid[moveLoc.row][moveLoc.col].threats[j].origin;
            //gs.bug << "Move loc: " << moveLocations[i] << " has threat at origin " << origin << std::endl;
        }
    }

    // Record all unique threats in a single vector
    vector<Threat> threats;
    for (i=0; i<(int)moveLocations.size(); ++i)
    {
        Location moveLoc = moveLocations[i];
        vector<Threat> ts = gs.grid[moveLoc.row][moveLoc.col].threats;
        for (int j=0; j<(int)ts.size(); ++j)
        {
            bool found = false;
            Threat t = ts[j];
            for (int k=0; k<(int)threats.size(); ++k)
            {
                if (t.origin == threats[k].origin)
                {
                    found = true;
                    break;
                }
            }
            
            if (found == false)
                threats.push_back(t);

        }

    }

    // Record the outDegree of every threat
    vector<int> threatOutDegree = vector<int>(threats.size(), 0);
    for (i=0; i<(int)threats.size(); ++i)
    {
        for (int j=0; j<(int)moveLocations.size(); ++j)
        {
            Location mLoc = moveLocations[j];
            vector<Threat> ts = gs.grid[mLoc.row][mLoc.col].threats;
            for (int k=0; k<(int)ts.size(); ++k)
            {
                if (ts[k].origin==threats[i].origin)
                {
                    //gs.bug << "increasing threatOutDegree " << std::endl;
                    threatOutDegree[i]++;
                    
                }
            }
            
        }
    }

    int score = 0;
    
    // Start evaluating moves
    i = 0;
    for (aIt = antGroup.begin(); aIt != antGroup.end(); ++aIt)
    {
        gs.myAnts[*aIt].move.safe = true;
        gs.myAnts[*aIt].move.unthreatened = true;
        //bool safe = true;
        //bool unthreatened = true;

        Location moveLoc = moveLocations[i];
        vector<Threat> ts = gs.grid[moveLoc.row][moveLoc.col].threats;
        for (int j=0; j<(int)ts.size(); ++j)
        {

            if (gs.myAnts[*aIt].move.safe == false)
                break;

            if (gs.myAnts[*aIt].move.unthreatened == false)
                break;

            Threat t = ts[j];
            for (int k=0; k<(int)threats.size(); ++k)
            {
                if ( t.origin == threats[k].origin)
                {
                    if (moveLocOutDegree[i] == threatOutDegree[k])
                    {
                        gs.myAnts[*aIt].move.unthreatened = false;
                        break;
                    }
                    else if (moveLocOutDegree[i] > threatOutDegree[k])
                    {
                        //gs.bug << "move out degree: " << moveLocOutDegree[i] << std::endl;
                        //gs.bug << "threat out degree: " << threatOutDegree[k] << std::endl;
                        gs.myAnts[*aIt].move.safe = false;
                        break;
                    }
                }
            } 
        }

        if (gs.myAnts[*aIt].move.onPath == false)
        {
            //gs.bug << "Ant " << gs.myAnts[*aIt].location << " with direction " << gs.myAnts[*aIt].move.direction << " is not onPath" << std::endl;
            score += 1;
        }

        if (gs.myAnts[*aIt].move.unthreatened == false)
        {
            //gs.bug << "Ant " << gs.myAnts[*aIt].location << " with direction " << gs.myAnts[*aIt].move.direction << " is not unthreatened" << std::endl;
            Task t = gs.myAnts[*aIt].task;
            Location antLoc = gs.myAnts[*aIt].location;
            Location dest = gs.getLocation(gs.myAnts[*aIt].move.origin, gs.myAnts[*aIt].move.direction);

            if ((t.description == INTERCEPT) &&
                (antLoc.manhattanDistance(t.destination) >= gs.grid[dest.row][dest.col].threats.front().origin.manhattanDistance(t.destination)))
            {
                // don't change the score when a single enemy is closer or just as close to the friendly hill
            }              
            else if ((t.description == GATHER) &&
                     (antLoc.manhattanDistance(t.destination) <= gs.grid[dest.row][dest.col].threats.front().origin.manhattanDistance(t.destination)))
            {
                // dont' change the score when a single enemy is further away or just as close to the food

                // but if there are more than two players then do add to the score
                if (gs.noPlayers >2)
                    score += 8;
            }
            else
                score += 8;
/*
            if (t.description != INTERCEPT)
            {
                if (!((t.description == GATHER) &&
                      (gs.myAnts[*aIt].location.manhattanDistance(t.destination) < 7)))
                score += 4;
            }
*/
        }
        else if (gs.myAnts[*aIt].move.safe == false)
        {
            //gs.bug << "Ant " << gs.myAnts[*aIt].location << " with direction " << gs.myAnts[*aIt].move.direction << " is not safe" << std::endl;

            score += 8;
        }
     
        ++i;   
    } 

    //gs.bug << "evaluateGroupMoves() 2" << std::endl;

    return score;

};


//makes the bots moves for the turn
void Bot::makeMoves()
{
    //gameState.bug << "-----------------------------------" << std::endl;
    //gameState.bug << "turn " << gameState.turn << ":" << std::endl;

     
    updateRealms(realms, gameState);
    tasks.clear();
    generateTasks(tasks, gameState);

    list<Realm>::iterator rIt = realms.begin();
    while (rIt != realms.end())
    {
        rIt->filterTasks(tasks, gameState);
        rIt->filterSurvivingAntsTasks(gameState);
        rIt->assignTasks(gameState);
        rIt->tasks.clear();
        ++rIt;
    }

    pathFinder.findPaths(gameState);
 
    vector<vector<bool> > antCollisions;
    makeAntCollisionMatrix(antCollisions, gameState);
 

    // Create ant groups based on collision graph
    list<list<int> > allGroups;
    makeAntGroups(antCollisions, allGroups);

    //gameState.bug << "size of groups: " << allGroups.size() << std::endl;
    makeCombatGroups(allGroups, gameState);
    //gameState.bug << "size of groups: " << allGroups.size() << std::endl;
    
    //gameState.printPaths();
   

    // Process and move each group
    list<list<int> >::iterator agIt;
    for (agIt = allGroups.begin(); agIt != allGroups.end(); ++agIt)
    {

        double t = gameState.timer.getTime();
        if (t > 450)
            return;

            list<int> &group = *agIt;
        
        //if (t < 450)
        //{
            //gameState.bug << "processing group..." << std::endl;
            makeConsistent(group, gameState);
            makeSafe(group, gameState);
        
            tabuSearch(group, gameState);
            validatePaths(group, gameState);
        //}

        // Move all the ants in the current group  
        while (!group.empty())
        {
            int ant = group.front();
            Move move = gameState.myAnts[ant].move;

            group.pop_front();

            Location moveDest = gameState.getLocation(move.origin, move.direction);

            bool canMove = true;
            list<int>::iterator gIt;
            for (gIt = group.begin(); gIt != group.end(); ++gIt)
            {
                if (gameState.myAnts[*gIt].location==moveDest)
                {
                    canMove = false;
                    break;
                }
            }

            if (canMove == true)
            {
                if (move.direction != NO_MOVE)
                {
                    gameState.makeMove(ant, move);
                }
            }
            else
            {
                group.push_back(ant);
            }
        }

        //gameState.bug << std::endl;

    }
 
 
    //gameState.bug << "time taken: " << gameState.timer.getTime() << "ms" << std::endl << std::endl;
};




//finishes the turn
void Bot::endTurn()
{
    if(gameState.turn > 0)
        gameState.reset();
    gameState.turn++;

    cout << "go" << endl;
};


