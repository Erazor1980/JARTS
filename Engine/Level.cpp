#include "Level.h"
#include <assert.h>

Level::Level()
{
}

void Level::drawTileGrid( Graphics& gfx, const Vei2& sp ) const
{
    assert( m_height > 0 && m_width > 0 && m_tileSize > 0 );

    for( int x = 0; x < m_width; ++x )
    {
        for( int y = 0; y < m_height; ++y )
        {
            RectF tile( { x * m_tileSize, y * m_tileSize }, { ( x + 1 ) * m_tileSize - 1, ( y + 1 ) * m_tileSize - 1 } );
            gfx.DrawRect( tile, Colors::Green );
        }
    }
}
