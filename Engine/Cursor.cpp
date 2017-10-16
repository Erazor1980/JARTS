#include "Cursor.h"
#include "SpriteEffect.h"

Cursor::Cursor( Graphics& gfx, const Mouse& mouse, const std::vector< Unit* >& vpUnits, const Level& level, const RectF& scrollRect )
    :
    m_mainSprite( "..\\images\\cursor\\cursor.bmp" ),
    m_forbiddenSprite( "..\\images\\cursor\\forbidden.bmp" ),
    m_arrowSprites( "..\\images\\cursor\\arrows.bmp" ),
    m_gfx( gfx ),
    m_mouse( mouse ),
    m_vpUnits( vpUnits ),
    m_level (level ),
    m_scrollingRect( scrollRect )
{
    /* calculating rectangles for the arrow sprites (directions) */
    m_arrowWidth     = m_arrowSprites.GetWidth() / 8;
    m_arrowHeight    = m_arrowSprites.GetHeight();
    for( int i = 0; i < 8; ++i )
    {
        m_vArrowSpriteRects.emplace_back( i * m_arrowWidth, ( i + 1 ) * m_arrowWidth, 0, m_arrowHeight );
    }
}

void Cursor::update( const float dt, const Vei2& camPos )
{
    const Vei2 halfScreen( Graphics::halfScreenWidth, Graphics::halfScreenHeight );
    const Vei2 offset = camPos - halfScreen;

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
    
        RectF bb = u->getBoundigBox(); 
        m_rectFromUnit = RectF( bb.left - offset.x, bb.right - offset.x, bb.top - offset.y, bb.bottom - offset.y );
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
            RectF bb = u->getBoundigBox();
            m_rectFromUnit = RectF( bb.left - offset.x, bb.right - offset.x, bb.top - offset.y, bb.bottom - offset.y );
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

void Cursor::draw( const Vei2& camPos )
{
    if( !m_mouse.IsInWindow() )
    {
        return;
    }
    const int x = m_mouse.GetPosX();
    const int y = m_mouse.GetPosY();
    RectF r = m_scrollingRect.getNormalized();

    if( !r.Contains( m_mouse.GetPos() ) )
    {
        if( x < r.left )
        {
            if( y < r.top )
            {
                m_gfx.DrawSprite( 0, 0, m_vArrowSpriteRects[ ( int )Direction::UP_LEFT ], m_arrowSprites, SpriteEffect::Chroma( Colors::White ) );
            }
            else if( y > r.bottom )
            {
                m_gfx.DrawSprite( 0, Graphics::ScreenHeight - m_arrowWidth, m_vArrowSpriteRects[ ( int )Direction::DOWN_LEFT ], m_arrowSprites, SpriteEffect::Chroma( Colors::White ) );
            }
            else
            {
                m_gfx.DrawSprite( 5, y - m_arrowHeight / 2, m_vArrowSpriteRects[ ( int )Direction::LEFT ], m_arrowSprites, SpriteEffect::Chroma( Colors::White ) );
            }
        }
        else if( x > r.right )
        {
            if( y < r.top )
            {
                m_gfx.DrawSprite( Graphics::ScreenWidth - m_arrowWidth, 0, m_vArrowSpriteRects[ ( int )Direction::UP_RIGHT ], m_arrowSprites, SpriteEffect::Chroma( Colors::White ) );
            }
            else if( y > r.bottom )
            {
                m_gfx.DrawSprite( Graphics::ScreenWidth - m_arrowWidth, Graphics::ScreenHeight - m_arrowHeight,
                                  m_vArrowSpriteRects[ ( int )Direction::DOWN_RIGHT ], m_arrowSprites, SpriteEffect::Chroma( Colors::White ) );
            }
            else
            {
                m_gfx.DrawSprite( Graphics::ScreenWidth - m_arrowWidth, y - m_arrowHeight / 2, m_vArrowSpriteRects[ ( int )Direction::RIGHT ], m_arrowSprites, SpriteEffect::Chroma( Colors::White ) );
            }
        }
        else if( y < r.top )
        {
            m_gfx.DrawSprite( x - m_arrowWidth / 2, 0, m_vArrowSpriteRects[ ( int )Direction::UP ], m_arrowSprites, SpriteEffect::Chroma( Colors::White ) );
        }
        else
        {
            m_gfx.DrawSprite( x - m_arrowWidth / 2, Graphics::ScreenHeight - m_arrowHeight, m_vArrowSpriteRects[ ( int )Direction::DOWN ], m_arrowSprites, SpriteEffect::Chroma( Colors::White ) );
        }
        return;
    }

    const Vei2 halfScreen( Graphics::halfScreenWidth, Graphics::halfScreenHeight );
    const Vei2 offset = camPos - halfScreen;
        
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

        if( m_bSelectedGroundUnit && Tile::OBSTACLE == m_level.getTileType( x + offset.x, y + offset.y ) )
        {
            m_gfx.DrawSprite( x - m_forbiddenSprite.GetWidth() / 2, y - m_forbiddenSprite.GetHeight() / 2, m_forbiddenSprite, SpriteEffect::Chroma( Colors::White ) );
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
        m_gfx.DrawSprite( x, y, m_mainSprite, SpriteEffect::Chroma( { 255, 242, 0 } ) );
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

