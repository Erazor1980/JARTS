#pragma once
#include "Graphics.h"
#include <vector>

class PathFinder;

enum class Tile
{
    EMPTY,
    OBSTACLE
    //TODO add more here
};

class Level
{
public:
    Level();
    ~Level();

    void init();
    
    /* test functions for visualization and debugging */
    void drawTileGrid( Graphics& gfx, const Vei2& sp ) const;
    void drawPath( Graphics& gfx, std::vector< int > vPath );


private:
    /* level size in tiles */
    int m_width         = 6;
    int m_height        = 7;

    /* tile size in pixels */
    float m_tileSize    = 22;

    /* Level content */
    Tile* mp_content = nullptr;

    friend PathFinder;
};