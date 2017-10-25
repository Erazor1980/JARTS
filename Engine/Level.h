#pragma once
#include "Graphics.h"
#include <vector>
#include <assert.h>

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
    Level( const std::string& filename, const int actionBarWidth );
    ~Level();

    void init();
    int getTileIdx( const int x, const int y ) const
    {
        int xTile = int( x / m_tileSize );
        int yTile = int( y / m_tileSize );

        return yTile * m_widthInTiles + xTile;
    }
    int getTileIdx( const Vec2& pixPos ) const
    {
        return getTileIdx( ( int )pixPos.x, ( int )pixPos.y );
    }

    Tile getTileType( const int tileIdx ) const
    {
        assert( tileIdx >= 0 && tileIdx < m_width * m_height );
        return mp_content[ tileIdx ];
    }
    Tile getTileType( const int x, const int y ) const   /* pixel coordinates */
    {
        return getTileType( getTileIdx( x, y ) );
    }
    Vec2 getTileCenter( const int tileIdx ) const;  /* return tile center in pixel coordinates */
    Vec2 getTileCenter( const int x, const int y ) const
    {
        return getTileCenter( getTileIdx( x, y ) );
    }

    void draw( Graphics& gfx, const Vei2& camera, const bool drawGrid = false /* for debugging */ ) const;

    /* test functions for visualization and debugging */
    void drawTileGrid( Graphics& gfx, const Vei2& camera, const bool drawFreeTiles = false ) const;
    void drawPath( Graphics& gfx, const Vei2& camera, std::vector< int > vPath, const int currIdx = 0, const int startIdx = -1, const int targetIdx = -1 ) const;

    /* getter functions */
    int getTileSize() const
    {
        return m_tileSize;
    }
    int getWidthInTiles() const
    {
        return m_widthInTiles;
    }
    int getHeightInTiles() const
    {
        return m_heightInTiles;
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
    RectI getLevelRect() const
    {
        assert( m_bInitialized );

        return RectI( { 0, 0 }, m_widthInTiles * m_tileSize, m_heightInTiles * m_tileSize );
    }
    int getActionBarWidth() const
    {
        return m_actionBarWidth;
    }
private:
    bool m_bInitialized = false;

    /* level size in tiles */
    int m_widthInTiles;
    int m_heightInTiles;

    /* level size in pixels */
    int m_width;
    int m_height;
    int m_halfWidth;
    int m_halfHeight;

    /* tile size in pixels */
    int m_tileSize = 40;

    /* level content */
    Tile* mp_content = nullptr;

    /* level image */
    Surface m_lvlImg;

    /* action bar */
    int m_actionBarWidth;

    friend PathFinder;
};