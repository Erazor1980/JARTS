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
        //mp_parent = parent;
        m_parentIdx = parentIdx;
    }
    int m_H;      // Heuristic (Manhattan distance)
    int m_G;      // Movement cost
    int m_F;      // G + H
    int m_idx;      // index of the node in map
    
    //Node* mp_parent = nullptr;
    int m_parentIdx;    // index of the parent node, -1 -> start node
};


class PathFinder
{
public:
    PathFinder( const Level& lvl );
    ~PathFinder();

    void init( const Level& lvl );

    std::vector< int > getShortestPath( const int start_idx, const int target_idx );

private:
    Node getAndRemoveLowestFcostNode( std::vector< Node >& vNodes );
    bool listContainsIdxNode( std::vector< Node >& vNodes, const int idx, int& nodeFoundIdx );

    /* g calculation for a neighbour */
    int getMoveCosts( const int idx1, const int idx2 );

    /* return a vector with all valid (non obstacles!) indices of the current index (i.e. tile/node) */
    std::vector< int > getNeighbourIndices( const int curr_idx );

    /* dimensions of the current level/map in tiles */
    int m_width;
    int m_height;

    /* for each cell all h values for the other cells will be precomputed and stored here */
    int* mp_all_H_values = nullptr;

    /* pointer to content of the current map (to know where the obstacles are) */
    const Tile* mp_mapContent = nullptr;
};