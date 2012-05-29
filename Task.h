#ifndef TASK_H_
#define TASK_H_

#include "Location.h"

// Task types in descending order of priority
enum TaskDescription {RAZE, GATHER, EXPLORE, INTERCEPT, PATROL, NONE};

/*
    struct for representing ant tasks
*/
struct Task
{
    Location hill;
    Location destination;
    TaskDescription description;
    int priority; 
    int numAnts; // number of ants assigned to this task 

    Task()
    {
        description = NONE;
        priority = calcInitialPriority(description);
        numAnts = 0;
    }

    Task(Location dest, TaskDescription desc)
        : destination(dest), description(desc), priority(calcInitialPriority(desc)), numAnts(0)
    {
    }
    
    Task(Location h, Location dest, TaskDescription desc)
        : hill(h), destination(dest), description(desc), priority(calcInitialPriority(desc)), numAnts(0)
    {
    }

    Task( const Task &t )
        : hill(t.hill), destination(t.destination), description(t.description), priority(t.priority), numAnts(t.numAnts)
    {
    }

    ~Task()
    {
    }

    Task &operator=( const Task &t )
    {
        if(this == &t)
        return *this;

        destination = t.destination;
        description = t.description;
        priority = t.priority;
        numAnts = t.numAnts;

        return *this;
    }


    // Equal by location only
    bool operator==( const Task &t ) const
    {
        return destination == t.destination;
    }

    bool operator!=( const Task &t ) const
    {
        return !(destination == t.destination);
    }

private:

    int calcInitialPriority( TaskDescription desc )
    {
/*
        int count = (int)desc;
        int sum = 0;

        while(count >= 0)
        {
            sum += pow(2,count);
            --count;
        }

        return sum;
*/
        if(INTERCEPT)
            return 0;
        if(RAZE)
            return 1;
        if(GATHER)
            return 3;
        if(EXPLORE)
            return 5;
        
        return 7;
    }
};

#endif //TASK_H_


