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
    int getTileIdx( const int x, const int y );
    
    /* test functions for visualization and debugging */
    void drawTileGrid( Graphics& gfx, const Vei2& sp ) const;
    void drawPath( Graphics& gfx, std::vector< int > vPath, const int startIdx, const int targetIdx );

    /* getter functions */
    float getTileSize() const
    {
        return m_tileSize;
    }
    int getWidth() const
    {
        return m_width;
    }
    int getHeight() const
    {
        return m_height;
    }
    bool isInitialized() const
    {
        return m_bInitialized;
    }
private:
    bool m_bInitialized = false;

    /* level size in tiles */
    int m_width         = 20;
    int m_height        = 20;

    /* tile size in pixels */
    float m_tileSize    = 22;

    /* Level content */
    Tile* mp_content = nullptr;

    friend PathFinder;
};