#ifndef BOT_H_
#define BOT_H_

#include <list>
#include <vector>
//#include "Bot.h"
#include "Realm.h"
#include "AStar.h"
#include "GameState.h"
/*
    This struct represents your bot in the game of Ants
*/
struct Bot
{
    AStar pathFinder;
    GameState gameState;
    std::list<Realm> realms;
    std::list<Task> tasks;

    Bot();
    ~Bot();

    void playGame();    //plays a single game of Ants
    void makeMoves();   //makes moves for a single turn
    void endTurn();     //indicates to the engine that it has made its moves
    
    void generateTasks( list<Task> &tasks, GameState &gs );
    void updateRealms( list<Realm> &r, GameState &gs);
    list<int> bfs( vector<vector<bool> > &graph, int start );

    void makeAntCollisionMatrix( vector<vector<bool> > &mat, GameState &gs );
    void makeAntGroups( vector<vector<bool> > &collisions, list<list<int> > &groups);
    void makeCombatGroups( list<list<int> > &groups, GameState &gs );
    void makeConsistent( list<int> &antGroup, GameState &gs );
    void makeSafe( list<int> &antGroup, GameState &gs );
    bool isConsistent( list<int> &antGroup, int ant, GameState &gs );
    void tabuSearch( list<int> &antGroup, GameState &gs );
    int evaluateGroupMoves( list<int> &antGroup, GameState &gs );
    void validatePaths( list<int> &antGroup, GameState &gs );
    bool commonEnemy( list<int> &antGroup1, list<int> &antGroup2, GameState &gs );

};

#endif //BOT_H_
