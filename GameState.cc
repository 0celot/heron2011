#include <algorithm>
#include <cassert>

#include "GameState.h"


using namespace std;


//const int NO_MOVE = -1;

//constructor
GameState::GameState()
{
    gameover = 0;
    turn = 0;
    bug.open("./debug.txt");
};

//deconstructor
GameState::~GameState()
{
    bug.close();
};

//sets the state up
void GameState::setup()
{
    grid = vector<vector<Square> >(rows, vector<Square>(cols, Square()));
};

//resets all non-water squares to land and clears the bots ant vector
void GameState::reset()
{
    enemyAnts.clear();
    myHills.clear();
    tFood.clear();
    for(int row=0; row<rows; row++)
    {
        for(int col=0; col<cols; col++)
        {
            if(!grid[row][col].isWater)
                grid[row][col].reset();
            
            grid[row][col].tickCounter();
        }
    }
};

//outputs move information to the engine
void GameState::makeMove(const int ant, const Move & m)
//void GameState::makeMove(const int ant, const Location &loc, int direction)
{
    //bug << "bug: o " << loc.row << " " << loc.col << " " << CDIRECTIONS[direction] << endl;
    cout << "o " << m << endl;
    //cout << "o " << loc.row << " " << loc.col << " " << CDIRECTIONS[direction] << endl;

    Location nLoc = getLocation(m.origin, m.direction);
    grid[nLoc.row][nLoc.col].ant = grid[m.origin.row][m.origin.col].ant;
    grid[m.origin.row][m.origin.col].ant = -1;

    myAnts[ant].location = nLoc;
};



//returns the new location from moving in a given direction with the edges wrapped
Location GameState::getLocation(const Location &loc, int direction) const
{
    if (direction == NO_MOVE)
        return Location(loc.row, loc.col);

    return Location( (loc.row + DIRECTIONS[direction][0] + rows) % rows,
                     (loc.col + DIRECTIONS[direction][1] + cols) % cols );
};


int GameState::getAntIndexFromLocation(const Location &loc) const
{
    for (int i=0; i<(int)myAnts.size(); ++i)
    {
        if (myAnts[i].location == loc)
            return i;
    } 

    return -1;
};

/*
    This function will update update the lastSeen value for any squares currently
    visible by one of your live ants.
*/
void GameState::updateVisionInformation()
{
    std::queue<Location> locQueue;
    Location sLoc, cLoc, nLoc;


    for (int a=0; a<(int) myAnts.size(); a++)
    {

        sLoc = myAnts[a].location;
        
        for (int j=0; j<(int)viewRange.size(); ++j)
        {
            cLoc = sLoc.absoluteFromRelative(viewRange[j]);
            grid[cLoc.row][cLoc.col].isVisible = 1;
            grid[cLoc.row][cLoc.col].state = VISIBLE;
            grid[cLoc.row][cLoc.col].resetCounter();
        }

    }

};


void GameState::updateThreatInformation()
{
    vector<Threat>::iterator tIt;

    for (int i=0; i<(int)enemyAnts.size(); ++i)
    {
        for (int j=0; j<(int)threatRange.size(); ++j)
        {
            // Threat location is the square that is being threatened
            Location lc = enemyAnts[i].absoluteFromRelative(threatRange[j].location);

            // Don't add threat to water
            if (grid[lc.row][lc.col].isWater)
                continue;

            // Threat origin is one of the 5 possible move positions for this enemy ant
            Location org = enemyAnts[i].absoluteFromRelative(threatRange[j].origin);

            // Threat key is the current position of the enemy ant
            Location ky = enemyAnts[i].absoluteFromRelative(threatRange[j].key);

            Threat t = Threat(lc,org,ky);

            // Has this enemy ant already been identified as a threat
            // due to one of its other possible move positions?                
            tIt = find(grid[lc.row][lc.col].threats.begin(), 
                       grid[lc.row][lc.col].threats.end(), 
                       t);

            // Increase threat total when it's a new enemy ant
            if (tIt==grid[lc.row][lc.col].threats.end())
                grid[lc.row][lc.col].threatTotal++;

            grid[lc.row][lc.col].threats.push_back(t);  

        }
    } 
};

void GameState::updateEnemyInformation()
{
    for (int i=0; i<(int)myAnts.size(); ++i)
    {

        // Reset the enemies list
        if (!myAnts[i].enemies.empty())
            myAnts[i].enemies.clear();

       
        // Check the current square
        Location loc = myAnts[i].location;
        for (int j=0; j<(int)grid[loc.row][loc.col].threats.size(); ++j)
        {
            Threat t = grid[loc.row][loc.col].threats[j];
            myAnts[i].enemies.push_back(t.key);
        } 

        // Check each adjacent square
        for (int j=0; j<4; ++j)
        {
            Location loc2 = getLocation(loc, j);
        
            for (int k=0; k<(int)grid[loc2.row][loc2.col].threats.size(); ++k)
            {
                Threat t = grid[loc2.row][loc2.col].threats[k];

                list<Location>::iterator eIt;
                eIt = find(myAnts[i].enemies.begin(),
                           myAnts[i].enemies.end(), t.key);

                if (eIt == myAnts[i].enemies.end())   
                    myAnts[i].enemies.push_back(t.key);
            } 
        }

        // sort the enemies list
        myAnts[i].enemies.sort();
        //bug << "Ant " << myAnts[i].location << " has " << myAnts[i].enemies.size() << " enemies." << std::endl;

    }
};



void GameState::findAntCollisions( vector<vector<bool> > &mat, double radius2 )
{ 
    int sz = myAnts.size();
    //vector<vector<bool> > antCollisions = vector<vector<bool> >(sz, vector<bool>( sz, false));
    //vector<vector<bool> > antCollisions = vector<vector<bool> >(sz, vector<bool>(false));
    mat = vector<vector<bool> >(sz, vector<bool>(sz, false));

    for (int i=0; i<sz; ++i)
    {
        for (int j=0; j<sz; ++j)
        //for (int j=0; j<i; ++j)
        {
            assert(i >= 0 && i < sz);
            assert(j >= 0 && j < sz);
            mat[i][j] = myAnts[i].collide(myAnts[j], radius2);
            //antCollisions[i][j] = myAnts[i].collide(myAnts[j], radius2);
            //antCollisions[i].push_back(myAnts[i].collide(myAnts[j], radius2));
        }
    }

    //return antCollisions;

};


// Make a vector of Locations surrounding a point within the given squared radius
// The resulting locations are relative to a location at [0,0]
void GameState::makeRange( vector<Location> &range, double radius2 )
{
    //bug << "radius2: " << radius2 << std::endl;  
    int numRows = sqrt(radius2) + 1;
    //bug << "numRows: " << numRows << std::endl; 

    int numCols = numRows;

    Location center = Location(0,0);
    range.push_back(center);

    for (int i=0; i<numRows; ++i)
    {
        for (int j=0; j<numCols; ++j)
        {
            if (i==0 && j==0)
                continue;

            Location loc = Location(i,j);

            if (center.distance2(loc)<=radius2)
            {
                //bug << loc << std::endl;
                range.push_back(loc);

                if (i!=0)
                {
                    loc = Location(-i,j);
                    range.push_back(loc);
                    //bug << loc << std::endl;
                }
                if (j!=0)
                {
                    loc = Location(i,-j);
                    range.push_back(loc);
                    //bug << loc << std::endl;
                }
                if ((i!=0) && (j!=0))
                {
                    loc = Location(-i,-j);
                    range.push_back(loc);
                    //bug << loc << std::endl;
                }
                
            }
        }
    }  
};

// Take an existing range and make a copy that accounts for possible
// movement of enemy ants, thus adding one square on each row/column 
// in each direction
vector<Threat> GameState::makeThreatRange( const vector<Location> &range )
{

    //bug << "original range: ";
    //for (int i=0; i<(int)range.size(); ++i)
    //    bug << range[i] << " : ";
    //bug << std::endl;

    vector<Threat> tRange;
    
    // Add the central squares
    for (int i=0; i<(int)range.size(); ++i)
    {
        tRange.push_back(Threat(range[i], Location(0,0), Location(0,0)));
    }

    // Add squares going up
    for (int i=0; i<(int)range.size(); ++i)
    {
        Location loc = Location( range[i].row - 1, range[i].col );

        bool present = false; 
        for (int j=0; j<(int)tRange.size(); ++j)
        {
            if (loc == tRange[j].location)
            {
                present = true;
                break;
            }
        }

        if (present == false)
        {
            tRange.push_back(Threat(loc, Location(-1,0), Location(0,0)));
            //tRange.push_back(loc);
        }
    }
    // Add squares going down
    for (int i=0; i<(int)range.size(); ++i)
    {
        Location loc = Location( range[i].row + 1, range[i].col );

        bool present = false; 
        for (int j=0; j<(int)tRange.size(); ++j)
        {
            if (loc == tRange[j].location)
            {
                present = true;
                break;
            }
        }

        if (present == false)
        {
            //tRange.push_back(loc);
            tRange.push_back(Threat(loc, Location(1,0), Location(0,0)));
        }
    }
    // Add squares to the left
    for (int i=0; i<(int)range.size(); ++i)
    {
        Location loc = Location( range[i].row, range[i].col - 1);

        bool present = false; 
        for (int j=0; j<(int)tRange.size(); ++j)
        {
            if (loc == tRange[j].location)
            {
                present = true;
                break;
            }
        }

        if (present == false)
        {
            //tRange.push_back(loc);
            tRange.push_back(Threat(loc, Location(0,-1), Location(0,0)));
        }
    } 
    // Add squares to the right
    for (int i=0; i<(int)range.size(); ++i)
    {
        Location loc = Location( range[i].row, range[i].col + 1);

        bool present = false; 
        for (int j=0; j<(int)tRange.size(); ++j)
        {
            if (loc == tRange[j].location)
            {
                present = true;
                break;
            }
        }

        if (present == false)
        {
            //tRange.push_back(loc);
            tRange.push_back(Threat(loc, Location(0,1), Location(0,0)));
        }
    }


    //bug << "threat range: ";
    //for (int i=0; i<(int)tRange.size(); ++i)
    //    bug << tRange[i] << " : ";
    //bug << std::endl;

    return tRange;
};



void GameState::updateFoodInformation()
{
        // This code added to preserve food that is no longer visible but may
        // still exist.
        for (int i=0; i<(int)food.size(); ++i)
        {
            bool disappeared = true;

            for (int j=0; j<(int)tFood.size(); ++j)
            {
                if (tFood[j] == food[i])
                {
                    disappeared = false;

                    tFood.erase(tFood.begin() + j);
                    break;
                }
            }

            if (disappeared == true)
            {
                // If that square is no longer visible the food 
                // may actually still be there so only erase it
                // if we see an empty square
                Location f = food[i];
 
                if (grid[f.row][f.col].isVisible == true)
                {
                    food.erase(food.begin() + i);
                    i--;
                }
            }
        }

        // Now add the new food
        for (int i=0; i<(int)tFood.size(); ++i)
            food.push_back(tFood[i]);

};

// For now, only update enemy hills information
void GameState::updateHillsInformation()
{
        // This code added to preserve enemy hills that are no longer visible but may
        // still exist.
        for (int i=0; i<(int)enemyHills.size(); ++i)
        {
            bool disappeared = true;

            for (int j=0; j<(int)tHills.size(); ++j)
            {
                if (tHills[j] == enemyHills[i])
                {
                    disappeared = false;

                    tHills.erase(tHills.begin() + j);
                    break;
                }
            }

            if (disappeared == true)
            {
                // If that square is no longer visible the hill 
                // may actually still be there so only erase it
                // if we see an empty square
                Location f = enemyHills[i];
 
                if (grid[f.row][f.col].isVisible == true)
                {
                    enemyHills.erase(enemyHills.begin() + i);
                    i--;
                }
            }
        }

        // Now add the new food
        for (int i=0; i<(int)tHills.size(); ++i)
            enemyHills.push_back(tHills[i]);

};



void GameState::printPaths()
{

    cout << "v setLineWidth 1" << endl;

    for (int i=0; i<(int)myAnts.size(); ++i)
    {

        if ( myAnts[i].task.description==GATHER )
            cout << "v setLineColor 0 255 0 255" << endl;
        else if ( myAnts[i].task.description==EXPLORE )
            cout << "v setLineColor 0 0 255 255" << endl;
        else if ( myAnts[i].task.description==RAZE )
            cout << "v setLineColor 255 0 0 255" << endl;
        else
            cout << "v setLineColor 255 255 255 255" << endl;

        list<Location>::iterator pIt = myAnts[i].path.begin();
        list<Location>::iterator pIt2 = myAnts[i].path.begin();
        ++pIt2;
        while (pIt2 != myAnts[i].path.end())
        {
            int row1 = pIt->row;
            int col1 = pIt->col;
            int row2 = pIt2->row;
            int col2 = pIt2->col;
            cout << "v line " << row1 << " " << col1 << " " << row2 << " " << col2 << endl;
            ++pIt;
            ++pIt2;
        }
    }
};

void GameState::printThreats()
{
    for (int i=0; i<rows; ++i)
    {
        for (int j=0; j<cols; ++j)
        {
            int color = grid[i][j].threatTotal * 20;
            if(color > 255)
                color = 255;

            if( color > 0 )
            {
                cout << "v setFillColor " << color << " 0 0 0.5" << std::endl; 
                cout << "v tile " << i << " " << j <<std::endl;
            }
        }
    }
}


/*
    This is the output function for a state. It will add a char map
    representation of the state to the output stream passed to it.

    For example, you might call "cout << state << endl;"
*/
ostream& operator<<(ostream &os, const GameState &state)
{
    for(int row=0; row<state.rows; row++)
    {
        for(int col=0; col<state.cols; col++)
        {
            if(state.grid[row][col].isWater)
                os << '%';
            else if(state.grid[row][col].isFood)
                os << '*';
            else if(state.grid[row][col].isHill)
                os << (char)('A' + state.grid[row][col].hillPlayer);
            else if(state.grid[row][col].ant >= 0)
                os << (char)('a' + state.grid[row][col].ant);
            else if(state.grid[row][col].state==VISIBLE)
                os << '.';
            else if(state.grid[row][col].state==CHARTED)
                os << '#';
            else
                os << '?';
        }
        os << endl;
    }

    return os;
};

//input function
istream& operator>>(istream &is, GameState &state)
{
    int row, col, player;
    string inputType, junk;

    //finds out which turn it is
    while(is >> inputType)
    {
        if(inputType == "end")
        {
            state.gameover = 1;
            break;
        }
        else if(inputType == "turn")
        {
            is >> state.turn;
            break;
        }
        else //unknown line
            getline(is, junk);
    }

    if(state.turn == 0)
    {
        //reads game parameters
        while(is >> inputType)
        {
            if(inputType == "loadtime")
                is >> state.loadtime;
            else if(inputType == "turntime")
                is >> state.turntime;
            else if(inputType == "rows")
            {
                is >> state.rows;
                Location::numRows = state.rows;
            }
            else if(inputType == "cols")
            {
                is >> state.cols;
                Location::numCols = state.cols;
            }
            else if(inputType == "turns")
                is >> state.turns;
            else if(inputType == "player_seed")
            {
                is >> state.seed;
                state.utility.r += state.seed;
                //state.bug << "setting seed: " << state.utility.r << std::endl; 
            }
            else if(inputType == "viewradius2")
            {
                is >> state.viewradius2;
                Ant::viewradius2 = state.viewradius2;
                //state.viewradius = sqrt(state.viewradius2);
            }
            else if(inputType == "attackradius2")
            {
                is >> state.attackradius2;
                Ant::attackradius2 = state.attackradius2;
                //state.attackradius = sqrt(state.attackradius2);
            }
            else if(inputType == "spawnradius2")
            {
                is >> state.spawnradius2;
                Ant::spawnradius2 = state.spawnradius2;
                //state.spawnradius = sqrt(state.spawnradius2);
            }
            else if(inputType == "ready") //end of parameter input
            {
                state.timer.start();
                break;
            }
            else    //unknown line
                getline(is, junk);
        }
    }
    else
    {
        
        std::vector<Ant> tAnts;

        //reads information about the current turn
        while(is >> inputType)
        {
            if(inputType == "w") //water square
            {
                is >> row >> col;
                state.grid[row][col].isWater = 1;
            }
            else if(inputType == "f") //food square
            {
                is >> row >> col;
                state.grid[row][col].isFood = 1;
                //state.food.push_back(Location(row, col));
                state.tFood.push_back(Location(row, col));
            }
            else if(inputType == "a") //live ant square
            {
                is >> row >> col >> player;
                state.grid[row][col].ant = player;
                if(player == 0)
                {
                    tAnts.push_back(Location(row, col));
                }
                else
                    state.enemyAnts.push_back(Location(row, col));
            }
            else if(inputType == "d") //dead ant square
            {
                is >> row >> col >> player;
                state.grid[row][col].deadAnts.push_back(player);
            }
            else if(inputType == "h")
            {
                is >> row >> col >> player;
                state.grid[row][col].isHill = 1;
                state.grid[row][col].hillPlayer = player;
                if(player == 0)
                    state.myHills.push_back(Location(row, col));
                else
                    state.tHills.push_back(Location(row, col));
                    //state.enemyHills.push_back(Location(row, col));

            }
            else if(inputType == "players") //player information
                is >> state.noPlayers;
            else if(inputType == "scores") //score information
            {
                state.scores = vector<double>(state.noPlayers, 0.0);
                for(int p=0; p<state.noPlayers; p++)
                    is >> state.scores[p];
            }
            else if(inputType == "go") //end of turn input
            {
                if(state.gameover)
                    is.setstate(std::ios::failbit);
                else
                    state.timer.start();
                break;
            }
            else //unknown line
                getline(is, junk);
        }



        // This code added to preserve task assignments in surviving ants - bbm
        // Remove any ants that didn't survive last turn
        for (int i=0; i<(int)state.myAnts.size(); ++i)
        {    
            bool survived = false;

            for (int j=0; j<(int)tAnts.size(); ++j)
            {
                if (tAnts[j].location == state.myAnts[i].location)
                {
                    survived = true;
        
                    // remove tant because its a duplicate
                    tAnts.erase(tAnts.begin() + j);
                    break;
                } 
            }

            if (survived == false)
            {
                // remove the dead ant from myAnts
                state.myAnts.erase(state.myAnts.begin() + i);
                i--;
            }
        }

        // Now add the new ants
        for (int i=0; i<(int)tAnts.size(); ++i)
            state.myAnts.push_back(tAnts[i]); 


    }

    return is;
};
