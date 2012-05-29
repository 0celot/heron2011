#ifndef SQUARE_H_
#define SQUARE_H_

#include <vector>
#include "Location.h"
#include "Threat.h"

enum SquareState {UNCHARTED, CHARTED, VISIBLE};


//const int NO_MOVE = -1;


/*
    struct for representing a square in the grid.
*/
struct Square
{
    int threatTotal;
    int counter;
    SquareState state;
    bool isVisible, isWater, isHill, isFood;
    int ant, hillPlayer;
    std::vector<Threat> threats;
    std::vector<int> deadAnts;

    Square()
    {
        threatTotal = 0;
        counter = 0;
        state = UNCHARTED;
        isVisible = isWater = isHill = isFood = 0;
        ant = hillPlayer = -1;
    };

    //resets the information for the square except water information
    void reset()
    {
        threatTotal = 0;
        isVisible = 0;
        //if(state==CHARTED)
        //    ++counter;
        if(state==VISIBLE)
        {
        //    counter = 0;
            state = CHARTED;
        }
        isHill = 0;
        isFood = 0;
        ant = hillPlayer = -1;
        deadAnts.clear();
        threats.clear();
    };

    void resetCounter()
    {
        counter = 0;
    }

    void tickCounter()
    {
        ++counter;
    }
};

#endif //SQUARE_H_
