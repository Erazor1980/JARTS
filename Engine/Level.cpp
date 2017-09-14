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

int Level::getTileIdx( const int x, const int y )
{
    int xTile = int( x / m_tileSize );
    int yTile = int( y / m_tileSize );

    return yTile * m_width + xTile;
}

void Level::drawTileGrid( Graphics& gfx, const Vei2& sp ) const
{
    assert( m_height > 0 && m_width > 0 && m_tileSize > 0 );

    for( int x = 0; x < m_width; ++x )
    {
        for( int y = 0; y < m_height; ++y )
        {
            RectF tile( { x * m_tileSize, y * m_tileSize }, { ( x + 1 ) * m_tileSize - 1, ( y + 1 ) * m_tileSize - 1 } );

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

void Level::drawPath( Graphics& gfx, std::vector< int > vPath, const int startIdx, const int targetIdx )
{
    /* draw path */
    for( auto i : vPath )
    {
        const int x = i % m_width;
        const int y = i / m_width;

        RectF tile( { x * m_tileSize, y * m_tileSize }, { ( x + 1 ) * m_tileSize - 1, ( y + 1 ) * m_tileSize - 1 } );

        gfx.DrawCircle( ( int )tile.GetCenter().x, ( int )tile.GetCenter().y, 3, Colors::Magenta );
    }

    /* draw start and target */
    const int xS = startIdx % m_width;
    const int yS = startIdx / m_width;
    RectF start( { xS * m_tileSize, yS * m_tileSize }, { ( xS + 1 ) * m_tileSize - 1, ( yS + 1 ) * m_tileSize - 1 } );
    gfx.DrawRectBorder( start, 2, Colors::Red );

    const int xT = targetIdx % m_width;
    const int yT = targetIdx / m_width;
    RectF end( { xT * m_tileSize, yT * m_tileSize }, { ( xT + 1 ) * m_tileSize - 1, ( yT + 1 ) * m_tileSize - 1 } );
    gfx.DrawRectBorder( end, 2, Colors::Blue );
}
