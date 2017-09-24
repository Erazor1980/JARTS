#include "PathControlling.h"

PathControlling::PathControlling( Level& currLevel )
    :
    m_level( currLevel )
{
}

void PathControlling::update( std::vector< Unit >& vUnits, const float dt )
{
    std::vector< UnitInfo > vUnitInfos;
    for( int i = 0; i < vUnits.size(); ++i )
    {
        vUnitInfos.push_back( { i, vUnits[ i ].getNextTileIdx(), vUnits[ i ].getPosTileIdx(), vUnits[ i ].isGroundUnit() } );
    }

    for( auto& u : vUnits )
    {
        const int nextTile = u.getNextTileIdx();

        std::vector< int > vOccupiedNeighbourTiles;
        for( auto infos : vUnitInfos )
        {
            /* ground units */
            if( u.isGroundUnit() && infos.bIsGroundUnit )
            {
                /* all moving or waiting units checking all other (excluding themselves) */
                if( u.getState() == Unit::State::MOVING || u.getState() == Unit::State::WAITING )
                {
                    if( u.getPosTileIdx() != infos.currTileIdx )
                    {
                        if( nextTile == infos.nextTileIdx || nextTile == infos.currTileIdx )
                        {
                            /* get tile indices occupied by units */
                            vOccupiedNeighbourTiles = checkNeighbourhood( u, vUnitInfos );
                            break;
                        }
                    }
                }
            }
            /* air units */
            else if( !u.isGroundUnit() && !infos.bIsGroundUnit )
            {
                int deb = 0;
            }
        }

        if( !vOccupiedNeighbourTiles.empty() || u.getState() == Unit::State::WAITING )
        {
            u.recalculatePath( vOccupiedNeighbourTiles, dt );
        }
    }
    int deb = 0;
}

std::vector< int > PathControlling::checkNeighbourhood( const Unit& unit, const std::vector< UnitInfo >& vUnitInfos )
{
    std::vector< int > vOccupiedNeighbourTiles;
    const int width     = m_level.getWidth();
    const int height    = m_level.getHeight();

    const int currX     = unit.getPosTileIdx() % width;
    const int currY     = unit.getPosTileIdx() / width;

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

            if( tmpX >= 0 && tmpX < width && tmpY >= 0 && tmpY < height )
            {
                int idx = tmpY * width + tmpX;
                for( const auto& ui : vUnitInfos )
                {
                    if( idx == ui.currTileIdx || idx == ui.nextTileIdx )
                    {
                        vOccupiedNeighbourTiles.push_back( idx );
                    }
                }
            }
        }
    }
    
    return vOccupiedNeighbourTiles;
}
