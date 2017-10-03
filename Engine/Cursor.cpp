#include "Cursor.h"

Cursor::Cursor( Graphics& gfx, const Mouse& mouse, const std::vector< Unit >& vUnits, const std::vector< Unit >& vEnemies, const Level& level )
    :
    m_mainSprite( "..\\images\\cursor\\cursor.bmp" ),
    m_forbiddenSprite( "..\\images\\cursor\\forbidden.bmp" ),
    m_gfx( gfx ),
    m_mouse( mouse ),
    m_vUnits( vUnits ),
    m_vEnemies( vEnemies),
    m_level (level )
{
}

void Cursor::update( const float dt )
{
    /* check if at least one unit is selected */
    m_bUnitSelected = false;
    for( const auto& u : m_vUnits )
    {
        if( u.isSelected() )
        {
            m_bUnitSelected         = true;
            m_bSelectedGroundUnit   = u.isGroundUnit();
            break;
        }
    }

    /* check for mouse over unit */
    Vec2 mp = m_mouse.GetPos();
    m_bMouseOverUnit = false;
    for( const auto &u : m_vUnits )
    {
        m_rectFromUnit = u.getBoundigBox();
        if( m_rectFromUnit.Contains( mp ) )
        {
            m_bMouseOverUnit = true;
            break;
        }
    }

    if( m_bMouseOverUnit )
    {
        m_cursorBlinkTime += dt;
        if( m_cursorBlinkTime >= m_cursorBlinkDelta )
        {
            m_bCursorBlinkShow = !m_bCursorBlinkShow;
            m_cursorBlinkTime = 0;
        }
    }

    /* movement / attack animations */
    if( m_bUnitSelected )
    {
        advanceAnimation();
        m_animationTime += dt;
        if( m_animationTime >= m_animationDelta )
        {
            m_animationIdx++;
            if( m_animationIdx >= m_maxAnimations )
            {
                m_animationIdx = 0;
            }
            m_animationTime = 0;
        }
    }
}

void Cursor::draw()
{
    if( !m_mouse.IsInWindow() )
    {
        return;
    }

    const int x = m_mouse.GetPosX();
    const int y = m_mouse.GetPosY();

    if( m_bMouseOverUnit )
    {
        if( m_bCursorBlinkShow )
        {
            m_gfx.DrawRectCorners( m_rectFromUnit, Colors::White );
        }
        return;
    }

    if( m_bUnitSelected )
    {
        if( m_bSelectedGroundUnit && Tile::OBSTACLE == m_level.getTileType( x, y ) )
        {
            m_gfx.DrawSprite( x - m_forbiddenSprite.GetWidth() / 2, y - m_forbiddenSprite.GetHeight() / 2, m_forbiddenSprite, Colors::White );
            m_animationIdx = 0;
            m_animationTime = 0;
            return;
        }
        
        m_gfx.DrawRectCorners( m_animationRect, Colors::Green );
        m_gfx.DrawCircle( m_mouse.GetPos(), 4, Colors::Green );
        return;
    }

    if( !m_bMouseOverUnit )
    {
        m_gfx.DrawSprite( x, y, m_mainSprite, { 255, 242, 0 } );
        m_cursorBlinkTime = 0;
        m_bCursorBlinkShow = true;
    }
}

void Cursor::advanceAnimation()
{
    const float size = 30.0f - m_animationIdx * 3;
    Vec2 topLeft = m_mouse.GetPos() - Vec2( size / 2, size / 2 );
    m_animationRect = RectF( topLeft, size, size );
}

