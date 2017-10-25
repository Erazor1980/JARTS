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
}

void ActionBar::draw( Graphics& gfx ) const
{
#if _DEBUG
    RectI r( Graphics::ScreenWidth - m_width, Graphics::ScreenWidth, 0, Graphics::ScreenHeight );
    gfx.DrawRect( r, Colors::LightGray );
#else
    gfx.DrawSprite( Graphics::ScreenWidth - m_width, 0, m_img, SpriteEffect::Copy{} );
#endif
}
