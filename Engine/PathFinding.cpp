#include <assert.h>
#include "PathFinding.h"

PathFinder::PathFinder( const Level& lvl )
{
    init( lvl );
}

PathFinder::~PathFinder()
{
    if( mp_all_H_values )
    {
        delete[] mp_all_H_values;
    }
    mp_all_H_values = nullptr;
}

void PathFinder::init( const Level& lvl )
{
    assert( lvl.m_width > 0 && lvl.m_height > 0 );
    m_width     = lvl.m_width;
    m_height    = lvl.m_height;
    mp_mapContent = lvl.mp_content;

    if( mp_all_H_values )
    {
        delete[] mp_all_H_values;
    }
    const int num_cells = m_width * m_height;
    mp_all_H_values = new int[ num_cells * num_cells ];     // because we need for each cell all h values from the other cells

    /////////////////////////////////
    //// PRECALCULATION h values ////
    /////////////////////////////////
    int* curr_block = mp_all_H_values;   // pointer to the the memory block for current cell h values
    for( int i = 0; i < num_cells; ++i )
    {
        /* move pointer to the memory block of the next cell */
        curr_block = &mp_all_H_values[ i * num_cells ];

        /* coordinates of the current cell (we are calculating h values for) */
        const int currX = i % m_width;
        const int currY = i / m_width;

        for( int y = 0; y < m_height; ++y )
        {
            const int row_offset = y * m_width;
            for( int x = 0; x < m_width; ++x )
            {
                int h = abs( x - currX ) + abs( y - currY );

                curr_block[ row_offset + x ] = h;
            }
        }
    }
}

std::vector< int > PathFinder::getShortestPath( const int start_idx, const int target_idx )
{
    assert( start_idx != target_idx );
    assert( start_idx >= 0 && start_idx < m_width * m_height );
    assert( target_idx >= 0 && target_idx < m_width * m_height );


    const int num_cells = m_width * m_height;
    std::vector< int > vPath;
    
    std::vector< Node > vOpenList;
    std::vector< Node > vClosedList;

    /* pointer to h values for the current target */
    int* current_H_values = &mp_all_H_values[ target_idx * num_cells ];

    Node startNode( start_idx, current_H_values[ start_idx ], 0 );
    vOpenList.push_back( startNode );

    ////////////////////////
    //// A* PATHFINDING ////
    ////////////////////////
    while( true )
    {
        Node currNode = getAndRemoveLowestFcostNode( vOpenList );

        vClosedList.push_back( currNode );

        /* target reached! get path indeces */
        if( target_idx == currNode.m_idx )
        {
            vPath.push_back( target_idx );

            int parentIdx = currNode.m_parentIdx;

            while( parentIdx != start_idx )
            {
                vPath.push_back( parentIdx );
                parentIdx = getParentIdxAndRemoveNodeFromList( vClosedList, parentIdx );
            }

            break;
        }

        std::vector< int > vNeighbours = getNeighbourIndices( currNode.m_idx );

        for( int n = 0; n < vNeighbours.size(); ++n )
        {
            const int idx = vNeighbours[ n ];
            int neighbourNodeIdx;   /* idx in the vOpenList of the node! not its map idx! */

            if( listContainsIdxNode( vClosedList, idx, neighbourNodeIdx ) )
            {
                continue;   // skip if node is in closed list
            }

            /* calculate g cost: currNode g + 10 (vertical/horizontal move) or + 14 (diagonal move) */
            const int g = currNode.m_G + getMoveCosts( currNode.m_idx, idx );

            
            if( !listContainsIdxNode( vOpenList, idx, neighbourNodeIdx ) )
            {               
                Node node( idx, current_H_values[ idx ], g, currNode.m_idx );

                vOpenList.push_back( node );
            }
            else
            {
                /* check if new path to neighbour is shorter */
                if( g < vOpenList[ neighbourNodeIdx ].m_G )
                {
                    vOpenList[ neighbourNodeIdx ].m_G = g;
                    vOpenList[ neighbourNodeIdx ].m_F = g + vOpenList[ neighbourNodeIdx ].m_H;
                    vOpenList[ neighbourNodeIdx ].m_parentIdx = currNode.m_idx;
                }
            }
        }
    }

    return vPath;
}

Node PathFinder::getAndRemoveLowestFcostNode( std::vector<Node>& vNodes )
{
    Node node;
    if( vNodes.size() == 1 )
    {
        node = vNodes[ 0 ];
        vNodes.clear();

        return node;
    }
    // find node with the lowest f cost
    int minIdx = 0;
    int minF = INT_MAX;
    for( int i = 0; i < vNodes.size(); ++i )
    {
        if( vNodes[ i ].m_F < minF )
        {
            minF = vNodes[ i ].m_F;
            minIdx = i;
        }
    }

    node = vNodes[ minIdx ];

    // remove node from list
    auto it = vNodes.begin();
    int cnt = 0;
    while( true )
    {
        if( cnt == minIdx )
        {
            vNodes.erase( it );
            break;
        }
        it++;
        cnt++;
    }

    return node;
}

int PathFinder::getParentIdxAndRemoveNodeFromList( std::vector<Node>& vNodes, const int idx )
{
    if( vNodes.empty() )
    {
        return -1;
    }

    int parentIdx = -1;
    // find and remove node from list
    auto it = vNodes.begin();
    while( it != vNodes.end() )
    {
        if( (*it).m_idx == idx )
        {
            parentIdx = ( *it ).m_parentIdx;
            vNodes.erase( it );
            break;
        }
        it++;
    }

    return parentIdx;
}

bool PathFinder::listContainsIdxNode( std::vector<Node>& vNodes, const int idx, int& nodeFoundIdx )
{
    if( vNodes.size() > 0 )
    {
        for( int i = 0; i < vNodes.size(); ++i )
        {
            if( idx == vNodes[ i ].m_idx )
            {
                nodeFoundIdx = i;
                return true;
            }
        }
    }
    nodeFoundIdx = -1;
    return false;
}

int PathFinder::getMoveCosts( const int idx1, const int idx2 )
{
    /* calculate g costs: 10 (vertical/horizontal move) or 14 (diagonal move) */

    const int x1 = idx1 % m_width;
    const int y1 = idx1 / m_width;

    const int x2 = idx2 % m_width;
    const int y2 = idx2 / m_width;

    // vertical/horizontal move
    if( x1 == x2 || y1 == y2 )
    {
        return 10;
    }
    // diagonal move
    else
    {
        return 14;
    }
    return 0;
}

std::vector< int > PathFinder::getNeighbourIndices( const int curr_idx )
{
    std::vector< int > vNeighbours;

    const int currX = curr_idx % m_width;
    const int currY = curr_idx / m_width;

    for( int x = -1; x < 2; ++x )
    {
        for( int y = -1; y < 2; ++y )
        {
            if( x == 0 && y == 0 )
            {
                continue;   // center idx itself
            }
            int tmpX = currX + x;
            int tmpY = currY + y;

            if( tmpX >= 0 && tmpX < m_width && tmpY >= 0 && tmpY < m_height )
            {
                int idx = tmpY * m_width + tmpX;
                if( mp_mapContent[ idx ] == Tile::EMPTY )
                {
                    vNeighbours.push_back( idx );
                }
            }
        }
    }


    return vNeighbours;
}
