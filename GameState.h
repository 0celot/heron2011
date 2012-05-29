#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <stdint.h>

#include "Timer.h"
#include "Bug.h"
#include "Square.h"
#include "Location.h"
#include "Ant.h"
#include "Utility.h"
#include "Threat.h"

/*
    constants
*/
const int TDIRECTIONS = 4;
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };      //{N, E, S, W}

/*
    struct to store current game state information. This is distinct
    from the internal state of the bot while making a move.
*/
struct GameState
{
    /*
        Variables
    */
    int rows, cols,
        turn, turns,
        noPlayers;
    //double attackradius, spawnradius, viewradius;
    double attackradius2, spawnradius2, viewradius2;
    double loadtime, turntime;
    std::vector<double> scores;
    bool gameover;
    int64_t seed;
    Utility utility;

    std::vector<std::vector<Square> > grid;
    std::vector<Location> enemyAnts, myHills, enemyHills, food, tFood, tHills;
    std::vector<Ant> myAnts;
    std::vector<Location> viewRange, attackRange, spawnRange;
    std::vector<Threat> threatRange;

    //std::vector<std::vector<bool> > antSpawnCollisions;

    Timer timer;
    Bug bug;

    /*
        Functions
    */
    GameState();
    ~GameState();

    void setup();
    void reset();

    void makeMove(const int ant, const Move &m);
    //void makeMove(const int ant, const Location &loc, int direction);

    double distance(const Location &loc1, const Location &loc2);
    Location getLocation(const Location &startLoc, int direction) const;
    int getAntIndexFromLocation(const Location &loc) const;

    void updateVisionInformation();
    void updateFoodInformation();
    void updateHillsInformation();
    void updateThreatInformation();
    void updateEnemyInformation();


    void makeRange( vector<Location> &range, double radius2 );
    vector<Threat> makeThreatRange( const vector<Location> &range );
    void findAntCollisions( vector<vector<bool> > &mat, double radius2 );

    void printPaths();
    void printThreats();

};

std::ostream& operator<<(std::ostream &os, const GameState &state);
std::istream& operator>>(std::istream &is, GameState &state);

#endif //STATE_H_
