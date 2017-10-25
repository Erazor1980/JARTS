#include "ActionBar.h"
#include "SpriteEffect.h"

ActionBar::ActionBar()
    :
#if _DEBUG
    m_img( "..\\images\\debugImg.bmp" )
#else
    m_img( "..\\images\\actionBar.bmp" )
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

void ActionBar::draw( Graphics& gfx, Font& font, const Vec2& mouse_pos, const Level& level, const Vei2& camPos ) const
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
        const Vei2 halfScreen( Graphics::halfScreenWidth, Graphics::halfScreenHeight );
        const Vei2 offset = camPos - halfScreen;

        Tile tile   = level.getTileType( ( int )mouse_pos.x + offset.x, ( int )mouse_pos.y + offset.y );
        int tileIdx = level.getTileIdx( ( int )mouse_pos.x + offset.x, ( int )mouse_pos.y + offset.y );

        //level.getTileIdx
    }
}

void ActionBar::handleMouse( const Mouse::Event::Type& type, const Vec2& mouse_pos )
{
    /* buildings */
    if( mouse_pos.y < m_vBuildingRects.back().bottom )
    {
        if( type == Mouse::Event::Type::LPress )
        {
            for( int i = 0; i < m_vBuildingRects.size(); ++i )
            {
                if( m_vBuildingRects[ i ].Contains( mouse_pos ) )
                {
                    m_buildingType  = Building::Type( i );
                    m_bPlacing      = true;
                    int deb = 0;
                }
            }
        }        
    }

}
