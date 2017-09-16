#include "Level.h"
#include <assert.h>

Level::Level()
{
    init();
}

Level::~Level()
{
    if( mp_content )
    {
        delete[] mp_content;
        mp_content = nullptr;
    }
}

void Level::init()
{
    assert( m_height > 0 && m_width > 0 );

    if( mp_content )
    {
        delete[] mp_content;
    }
    mp_content = new Tile[ m_width * m_height ];

#if 1   /* random level */
    for( int i = 0; i < m_width * m_height; ++i )
    {
        if( rand() % 4 )
        {
            mp_content[ i ] = Tile::EMPTY;
        }
        else
        {
            mp_content[ i ] = Tile::OBSTACLE;
        }
    }
#else   /* controlled scenario */
    for( int i = 0; i < m_width * m_height; ++i )
    {
        mp_content[ i ] = Tile::EMPTY;
    }

    mp_content[ 16 ] = Tile::OBSTACLE;
    mp_content[ 26 ] = Tile::OBSTACLE;
    mp_content[ 28 ] = Tile::OBSTACLE;
    mp_content[ 34 ] = Tile::OBSTACLE;
#endif

    m_bInitialized = true;
}

int Level::getTileIdx( const int x, const int y ) const
{
    int xTile = int( x / m_tileSize );
    int yTile = int( y / m_tileSize );

    return yTile * m_width + xTile;
}

Vec2 Level::getTileCenter( const int tileIdx ) const
{
    const int xTile = tileIdx % m_width;
    const int yTile = tileIdx / m_width;

    const float x = xTile * m_tileSize + m_tileSize / 2.0f - 1;
    const float y = yTile * m_tileSize + m_tileSize / 2.0f - 1;

    return Vec2( x, y );
}

void Level::drawTileGrid( Graphics& gfx, const Vei2& sp ) const
{
    assert( m_height > 0 && m_width > 0 && m_tileSize > 0 );

    for( int x = 0; x < m_width; ++x )
    {
        for( int y = 0; y < m_height; ++y )
        {
            RectI tile( { x * m_tileSize, y * m_tileSize }, { ( x + 1 ) * m_tileSize - 1, ( y + 1 ) * m_tileSize - 1 } );

            if( Tile::EMPTY == mp_content[ y * m_width + x ] )
            {
                gfx.DrawRect( tile, Colors::Green );
            }
            else
            {
                gfx.DrawRect( tile, Colors::Red );
            }
        }
    }
}

void Level::drawPath( Graphics& gfx, std::vector< int > vPath, const int startIdx, const int targetIdx ) const
{
    /* draw path */
    for( auto i : vPath )
    {
        const int x = i % m_width;
        const int y = i / m_width;

        RectI tile( { x * m_tileSize, y * m_tileSize }, { ( x + 1 ) * m_tileSize - 1, ( y + 1 ) * m_tileSize - 1 } );

        gfx.DrawCircle( tile.GetCenter().x, tile.GetCenter().y, 3, Colors::Magenta );
    }

    /* draw start and target */
    if( startIdx >= 0 && targetIdx >= 0 )
    {
        const int xS = startIdx % m_width;
        const int yS = startIdx / m_width;
        RectI start( { xS * m_tileSize, yS * m_tileSize }, { ( xS + 1 ) * m_tileSize - 1, ( yS + 1 ) * m_tileSize - 1 } );
        gfx.DrawRectBorder( start, 2, Colors::Red );

        const int xT = targetIdx % m_width;
        const int yT = targetIdx / m_width;
        RectI end( { xT * m_tileSize, yT * m_tileSize }, { ( xT + 1 ) * m_tileSize - 1, ( yT + 1 ) * m_tileSize - 1 } );
        gfx.DrawRectBorder( end, 2, Colors::Blue );
    }
}
