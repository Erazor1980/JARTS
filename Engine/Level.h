#pragma once
#include "Graphics.h"

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
    /* test function for tile visualization */
    void drawTileGrid( Graphics& gfx, const Vei2& sp ) const;
private:
    /* level size in tiles */
    int m_width         = 20;
    int m_height        = 20;

    /* tile size in pixels */
    float m_tileSize    = 22;

    /* Level content */
    Tile* mp_content = nullptr;
};