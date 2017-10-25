#include "ActionBar.h"
#include "SpriteEffect.h"
#include "RectF.h"

ActionBar::ActionBar()
    :
#if _DEBUG
    m_img( "..\\images\\debugImg.bmp" )
#else
    m_img( "..\\images\\actionBar\\actionBar.bmp" )
#endif
{
#if _DEBUG
    m_width = 200;
#else
    m_width = m_img.GetWidth();
#endif
    const int size      = m_width / 2;
    const int startY    = 50;
    /* barracks (top left) */
    m_vBuildingRects.push_back( RectI( Graphics::ScreenWidth - m_width, Graphics::ScreenWidth - size, startY, startY + size ) );

    /* factory (top right) */
    m_vBuildingRects.push_back( RectI( Graphics::ScreenWidth - size, Graphics::ScreenWidth, startY, startY + size ) );
}

void ActionBar::draw( Graphics& gfx, Font& font, const Vec2& mousePos, const Level& level, const Vei2& camPos ) const
{
#if _DEBUG
    RectI r( Graphics::ScreenWidth - m_width, Graphics::ScreenWidth, 0, Graphics::ScreenHeight );
    gfx.DrawRectBorder( r, 4, Colors::LightGray );
#else
    gfx.DrawSprite( Graphics::ScreenWidth - m_width, 0, m_img, SpriteEffect::Copy{} );
#endif

    /* draw building rects */
    for( auto b : m_vBuildingRects )
    {
        gfx.DrawRectBorder( b, 2, Colors::Blue );
    }

    /* draw building names */
    font.DrawText( "B", m_vBuildingRects[ ( int )Building::Type::BARRACKS ].GetCenter() - Vei2( 8, 14 ), Colors::Blue, gfx );
    font.DrawText( "F", m_vBuildingRects[ ( int )Building::Type::FACTORY ].GetCenter() - Vei2( 8, 14 ), Colors::Blue, gfx );

    /* building placing */
    if( m_bPlacing )
    {
        if( mousePos.x >= Graphics::ScreenWidth - m_width )
        {

        }
        else
        {
            const Vei2 halfScreen( Graphics::halfScreenWidth, Graphics::halfScreenHeight );
            const Vei2 offset = camPos - halfScreen;

            const float tileSize = ( float )level.getTileSize();
            for( int x = 0; x < m_buildingSize.x; ++x )
            {
                for( int y = 0; y < m_buildingSize.y; ++y )
                {
                    RectF r     = level.getTileRect( mousePos + Vec2( x * tileSize, y * tileSize ), camPos );
                    Tile tile   = level.getTileType( ( int )mousePos.x + offset.x + x * ( int )tileSize, ( int )mousePos.y + offset.y + y * ( int )tileSize );
                    if( Tile::EMPTY == tile )
                    {
                        gfx.DrawRectBorder( r, 2, Colors::Green );
                    }
                    else
                    {
                        gfx.DrawRectBorder( r, 2, Colors::Red );
                    }
                }
            }
        }
    }
}

void ActionBar::handleMouse( const Mouse::Event::Type& type, const Vec2& mousePos )
{
    /* buildings */
    if( mousePos.y < m_vBuildingRects.back().bottom )
    {
        if( type == Mouse::Event::Type::LPress )
        {
            for( int i = 0; i < m_vBuildingRects.size(); ++i )
            {
                if( m_vBuildingRects[ i ].Contains( mousePos ) )
                {
                    m_buildingType  = Building::Type( i );
                    m_bPlacing      = true;
                    m_buildingSize  = Building::getSizeInTiles( m_buildingType );
                    int deb = 0;
                }
            }
        }
    }
}
