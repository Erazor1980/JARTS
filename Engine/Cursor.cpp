#include "Cursor.h"

Cursor::Cursor( Graphics& gfx, const Mouse& mouse, const std::vector< Unit* >& vpUnits, const Level& level )
    :
    m_mainSprite( "..\\images\\cursor\\cursor.bmp" ),
    m_forbiddenSprite( "..\\images\\cursor\\forbidden.bmp" ),
    m_gfx( gfx ),
    m_mouse( mouse ),
    m_vpUnits( vpUnits ),
    m_level (level )
{
}

void Cursor::update( const float dt )
{
    /* check if at least one unit is selected */
    m_bUnitSelected = false;
    for( const auto& u : m_vpUnits )
    {
        if( u->isSelected() )
        {
            m_bUnitSelected         = true;
            m_bSelectedGroundUnit   = u->isGroundUnit();
            break;
        }
    }

    /* check for mouse over (own) unit */
    Vec2 mp = m_mouse.GetPos();
    m_bMouseOverUnit = false;
    for( const auto &u : m_vpUnits )
    {
        if( u->getTeam() != Team::_A )
        {
            continue;
        }
        m_rectFromUnit = u->getBoundigBox();
        if( m_rectFromUnit.Contains( mp ) )
        {
            if( u->getTeam() != Team::_A )
            {
                m_bMouseOverEnemy = true;
            }
            m_bMouseOverUnit = true;
            break;
        }
    }

    m_bMouseOverEnemy = false;      /* check enemy units */
    if( !m_bMouseOverUnit )
    {
        for( const auto &u : m_vpUnits )
        {
            if( u->getTeam() == Team::_A )
            {
                continue;
            }
            m_rectFromUnit = u->getBoundigBox();
            if( m_rectFromUnit.Contains( mp ) )
            {
                m_bMouseOverUnit = true;
                m_bMouseOverEnemy = true;
                break;
            }
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
    
    if( m_bUnitSelected )
    {
        if( m_bMouseOverEnemy )
        {
            const int radius = 25 - m_animationIdx * 3;
            Vec2 tileCenter = m_level.getTileCenter( m_mouse.GetPosX(), m_mouse.GetPosY() );
            m_gfx.DrawCircleBorder( tileCenter, radius, Colors::Red );
            m_gfx.DrawCircleBorder( tileCenter, radius - 4, Colors::Red );
            m_gfx.DrawRectCorners( m_rectFromUnit, Colors::Red );
            return;
        }

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

    if( m_bMouseOverUnit )
    {
        if( m_bCursorBlinkShow )
        {
            m_gfx.DrawRectCorners( m_rectFromUnit, Colors::White );
        }
        return;
    }
    else
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

