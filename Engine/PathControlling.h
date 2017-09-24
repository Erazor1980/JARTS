#pragma once
#include "Level.h"
#include "Unit.h"

class PathControlling
{
public:
    PathControlling( Level& currLevel );

    void update( std::vector< Unit >& vUnits, const float dt );
private:
    struct UnitInfo
    {
        int idxInUnitVector;
        int nextTileIdx;
        int currTileIdx;
        bool bIsGroundUnit;
    };

    /* return indices from occupied tiles */
    std::vector< int > checkNeighbourhood( const Unit& unit, const std::vector< UnitInfo >& vUnitInfos );

    Level& m_level;
};