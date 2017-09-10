#pragma once
#include "Graphics.h"
#include "Vec2.h"

class Level
{
public:
    Level();

    /* test function for tile visualization */
    void drawTileGrid( Graphics& gfx, const Vei2& sp ) const;
private:
    /* level size in tiles */
    int m_width         = 20;
    int m_height        = 20;

    /* tile size in pixels */
    float m_tileSize    = 30;
};