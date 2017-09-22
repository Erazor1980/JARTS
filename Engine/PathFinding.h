#pragma once
#include "Level.h"
#include <vector>

struct Node
{
    Node() = default;
    Node( int idx, int h, int g, int parentIdx = -1 )
    {
        m_idx = idx;
        m_H = h;
        m_G = g;
        m_F = g + h;
        m_parentIdx = parentIdx;
    }
    int m_idx;    // index of the node in map

    int m_H;      // Heuristic (Manhattan distance)
    int m_G;      // Movement cost
    int m_F;      // G + H
        
    int m_parentIdx;    // index of the parent node, -1 -> start node
};


class PathFinder
{
public:
    PathFinder( const Level& lvl );
    ~PathFinder();

    void init( const Level& lvl );

    std::vector< int > getShortestPath( const int start_idx, const int target_idx, const int neighbour_unit_idx = -1 ); /* if there is a unit in the neighbour tile */

private:
    Node getAndRemoveLowestFcostNode( std::vector< Node >& vNodes );

    /* for following idx path after target is reached in A* */
    int getParentIdxAndRemoveNodeFromList( std::vector< Node >& vNodes, const int idx );   

    bool listContainsIdxNode( std::vector< Node >& vNodes, const int idx, int& nodeFoundIdx /* index in the vector vNodes (!!, NOT its map idx) of the found node */);

    /* g calculation for a neighbour */
    int getMoveCosts( const int idx1, const int idx2 );

    /* return a vector with all valid (non obstacles / non neighbour units!) indices of the current index (i.e. tile/node) */
    std::vector< int > getNeighbourIndices( const int curr_idx, const int neighbour_unit_idx = -1 );

    /* dimensions of the current level/map in tiles */
    int m_width;
    int m_height;

    /* for each cell all h values for the other cells will be precomputed and stored here */
    int* mp_all_H_values = nullptr;

    /* pointer to content of the current map (to know where the obstacles are) */
    const Tile* mp_mapContent = nullptr;
};