#include "PathFinding.h"
#include <assert.h>

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

    if( mp_all_H_values )
    {
        delete[] mp_all_H_values;
    }
    const int num_cells = m_width * m_height;
    mp_all_H_values = new int[ num_cells * num_cells ];     // because we need for each cell all h values from the other cells

    /////////////////////////////////
    //// PRECALCULATION h values ////
    /////////////////////////////////
    int* curr_cell = mp_all_H_values;   // pointer to the the memory block for current cell h values
    for( int i = 0; i < num_cells; ++i )
    {

    }

}
