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
