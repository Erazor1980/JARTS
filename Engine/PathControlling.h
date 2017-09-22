#pragma once
#include "Level.h"
#include "Unit.h"

class PathControlling
{
public:
    PathControlling( Level& currLevel );

    void update( std::vector< Unit >& vUnits );
private:
    Level& m_level;
};