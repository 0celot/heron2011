#include <list>
#include "AStar.h"

const int HEURISTIC = 4;

greater<SearchNode> nodeComp;

AStar::AStar()
{
}


AStar::AStar( int rows, int cols )
{
    inOpen = vector<vector<bool> >(rows, vector<bool>(cols, false));
    inClosed = vector<vector<bool> >(rows, vector<bool>(cols, false));
}


AStar::~AStar( )
{
}

void AStar::run( list<Location> &path, Location start, Location finish, GameState &gs )
{

    double t = gs.timer.getTime();

    SearchNode c = SearchNode(start);
    SearchNode n;

    c.gval = 0;
    int manhattan = start.manhattanDistance(finish);
    c.hval = HEURISTIC * manhattan;
    c.fval = c.gval + c.hval;
    c.parentLocation = Location(-1,-1); 

    addOpen(c);

    int timecheck = 0;
    while (!open.empty())
    {
        c = getBestOpen();

        if (c==finish)
            break;

        addClosed(c);
    
        for (int i=0; i<4; ++i)
        {
            n.location = gs.getLocation(c.location,i);
            n.parentLocation = c.location;

            int terrain_cost = 1;
            if (gs.grid[n.location.row][n.location.col].isWater == true)
                terrain_cost += 10000;
            else if ((gs.grid[n.location.row][n.location.col].isHill == true) &&
                     (gs.grid[n.location.row][n.location.col].hillPlayer == 0))
                terrain_cost += 10000;
            else if ((gs.grid[n.location.row][n.location.col].state == UNCHARTED) &&
                     (n.location != finish))
                terrain_cost += 10000;
        
            int cost = c.gval + terrain_cost;

            if (isInOpen(n))
            {
                vector<SearchNode>::iterator oIt = find(open.begin(), open.end(), n);
                if (cost < oIt->gval)
                {
                    open.erase(oIt);
                    inOpen[n.location.row][n.location.col] = false;
                    make_heap(open.begin(), open.end(), nodeComp);
                    ++openAdjusts;
                }
            }

            if (isInClosed(n))
            {
                vector<SearchNode>::iterator cIt = find(closed.begin(), closed.end(), n);
                if (cost < cIt->gval)
                {
                    closed.erase(cIt);
                    inClosed[n.location.row][n.location.col] = false;
                    closedAdjusts++;
                }
            }

            if (!(isInOpen(n) || isInClosed(n)))
            {
                n.gval = cost;
                n.hval = HEURISTIC * n.location.manhattanDistance(finish);
                n.fval = n.gval + n.hval;
                addOpen(n);
            }
        }

        visited++;

        timecheck++;
        if (timecheck > 100)
        {
            double t2 = gs.timer.getTime();
            if (t2 - t > 20)
                break;
            timecheck = 0;
        }
        // bail out if neccessary
        
        //if (gs.timer.getTime() > 300)
        //    break;

    }


    //gs.bug << "finish at: " << c.location << std::endl; 
    //gs.bug << "nodes visited: " << visited << std::endl << std::endl; 

    path.push_front(c.location);

    while(c.parentLocation != start)
    {
        c = *find(closed.begin(), closed.end(), SearchNode(c.parentLocation));
        path.push_front(c.location);
    }   


    double d_t = gs.timer.getTime() - t;
        gs.bug << manhattan 
        << ", " << path.size() 
        << ", " << visited
        << ", " << inserts
        << ", " << removes
        << ", " << memberships
        << ", " << openAdjusts
        << ", " << closedAdjusts
        << ", " << d_t << std::endl; 

}


void AStar::findPaths( GameState &gs )
{

    for (int i=0; i<(int)gs.myAnts.size(); ++i)
    {
        double t = gs.timer.getTime();
        if (t > 300)
            break;

        if (gs.myAnts[i].path.size() == 0 && gs.myAnts[i].task.description != NONE )
        {
            run(gs.myAnts[i].path, gs.myAnts[i].location, gs.myAnts[i].task.destination, gs);
            reset();
        }
    }

}

void AStar::reset( )
{
    SearchNode n;
    int sz = open.size();
    for(int i=0;i<sz;++i)
    {
        n = open[i];
        inOpen[n.location.row][n.location.col] = false;
    }
    open.clear();
    
    sz = closed.size();
    for(int i=0;i<sz;++i)
    {
        n = closed[i];
        inClosed[n.location.row][n.location.col] = false;
    }
    closed.clear();

    visited = 0;
    inserts = 0;
    removes = 0;
    memberships = 0;
    openAdjusts = 0;
    closedAdjusts = 0;
}


SearchNode AStar::getBestOpen( )
{
    SearchNode n = open.front();
    pop_heap(open.begin(), open.end(), nodeComp);
    open.pop_back(); 
    inOpen[n.location.row][n.location.col] = false;
    ++removes;

    return n;
}

void AStar::addOpen( const SearchNode &n )
{
    open.push_back(n);
    push_heap(open.begin(), open.end(), nodeComp);
    inOpen[n.location.row][n.location.col] = true;
    ++inserts;
}


void AStar::addClosed( const SearchNode &n )
{
    closed.push_back(n);
    inClosed[n.location.row][n.location.col] = true;
    ++inserts;
}


bool AStar::isInOpen( const SearchNode &n )
{
    ++memberships;
    return inOpen[n.location.row][n.location.col];
}

bool AStar::isInClosed( const SearchNode &n )
{
    ++memberships;
    return inClosed[n.location.row][n.location.col];
}

vector<SearchNode>::iterator AStar::findInOpen( const SearchNode &n )
{
    if (isInOpen(n))
    {
        for (vector<SearchNode>::iterator i=open.begin(); i!=open.end(); ++i)
        {
            if ((*i)==n)
                return i;
        }
    }
    return open.end();
}


vector<SearchNode>::iterator AStar::findInClosed( const SearchNode &n )
{
    if (isInClosed(n))
    {
        for (vector<SearchNode>::iterator i=closed.begin(); i!=closed.end(); ++i)
        {
            if ((*i)==n)
                return i;
        }
    }
    return closed.end();
}


