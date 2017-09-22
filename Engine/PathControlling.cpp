#include "PathControlling.h"

PathControlling::PathControlling( Level& currLevel )
    :
    m_level( currLevel )
{
}

void PathControlling::update( std::vector< Unit >& vUnits )
{
    struct UnitInfo
    {
        int idxInUnitVector;
        int nextTileIdx;
        int currTileIdx;
        bool bIsGroundUnit;
    };

    std::vector< UnitInfo > vUnitInfos;
    for( int i = 0; i < vUnits.size(); ++i )
    {
        vUnitInfos.push_back( { i, vUnits[ i ].getNextTileIdx(), vUnits[ i ].getPosTileIdx(), vUnits[ i ].isGroundUnit() } );
    }

    for( auto& u : vUnits )
    {
        const int nextTile = u.getNextTileIdx();
        for( auto infos : vUnitInfos )
        {
            if( u.isGroundUnit() != infos.bIsGroundUnit )   /* only ground vs ground or air vs air */
            {
                continue;
            }

            if( nextTile >= 0 && u.getPosTileIdx() != infos.currTileIdx )   /* all moving units checking all other (excluding themselves) */
            {
                if( nextTile == infos.nextTileIdx || nextTile == infos.currTileIdx )
                {
                    u.recalculatePath( nextTile );
                }
            }
        }
    }
    int deb = 0;
}
