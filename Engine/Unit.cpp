#include "Unit.h"
#include <assert.h>

Unit::Unit( const Vec2 pos_tile, const Level* const pLevel, PathFinder* const pPathFinder, const Surface& sprite, const std::vector< RectI >& spriteRects )
    :
    mp_level( pLevel ),
    mp_pathFinder( pPathFinder ),
    m_sprite( sprite ),
    m_vSpriteRects( spriteRects )
{
    assert( pLevel->isInitialized() );
    assert( pos_tile.x >= 0 && pos_tile.x < pLevel->getWidth()
            && pos_tile.y >= 0 && pos_tile.y < pLevel->getHeight() );
    assert( spriteRects.size() > 0 );

    m_size = pLevel->getTileSize();
    m_pos_tile  = pos_tile;
    m_pos.x     = pos_tile.x * m_size + m_size / 2 - 1;
    m_pos.y     = pos_tile.y * m_size + m_size / 2 - 1;

    m_halfSize = m_size / 2;
    m_bb = RectF( m_pos - Vec2( ( float )m_halfSize, ( float )m_halfSize ), m_pos + Vec2( ( float )m_halfSize + 1, ( float )m_halfSize + 1 ) );

    m_spriteDirection = ( Direction )( rand() % 8 );
}

void Unit::draw( Graphics& gfx, const bool drawPath ) const
{
    /* drawing current (remaining) path when selected */
    if( drawPath && m_bSelected && State::MOVING == m_state )
    {
        mp_level->drawPath( gfx, m_vPath, m_pathIdx );
    }

    if( m_bSelected )
    {
        gfx.DrawCircle( ( int )m_pos.x, ( int )m_pos.y, ( int )m_halfSize + 1, Colors::Blue );
    }

    gfx.DrawSprite( ( int )m_pos.x - m_halfSize, ( int )m_pos.y - m_halfSize, m_vSpriteRects[ ( int )m_spriteDirection ], m_sprite, Colors::White );


#if DEBUG_INFOS
    /* draw bounding box */
    gfx.DrawRectBorder( m_bb, 1, Colors::White );
#endif
}

void Unit::update( const Mouse::Event::Type& type, const Vec2& mouse_pos, const bool shift_pressed, const float dt )
{
    if( mp_level->getLevelRect().Contains( mouse_pos ) )
    {
        handleMouse( type, mouse_pos, shift_pressed );
    }

    if( State::MOVING == m_state )
    {
        calcDirection();

        m_vel = m_dir * m_speed;
        m_pos += m_vel * dt;
        m_bb = RectF( m_pos - Vec2( ( float )m_halfSize, ( float )m_halfSize ), m_pos + Vec2( ( float )m_halfSize + 1, ( float )m_halfSize + 1 ) );
    }
}

void Unit::select()
{
    m_bSelected = true;
}
void Unit::deselect()
{
    m_bSelected = false;
}

void Unit::handleMouse( const Mouse::Event::Type& type, const Vec2& mouse_pos, const bool shift_pressed )
{
    if( type == Mouse::Event::Type::LPress )
    {
        if( !m_bSelected )
        {
            /* check if we click inside the bounding box */
            if( m_bb.IsOverlappingWith( RectF( mouse_pos, 1, 1 ) ) )
            {
                m_bSelected = true;
            }
        }
        else
        {
            if( !shift_pressed )
            {
                m_bSelected = false;
            }
        }
    }
    else if( type == Mouse::Event::Type::RPress )
    {
        if( m_bSelected )
        {
            Tile targetTile = mp_level->getTileType( ( int )mouse_pos.x, ( int )mouse_pos.y );
            const int startIdx = ( int )m_pos_tile.y * mp_level->getWidth() + ( int )m_pos_tile.x;
            const int targetIdx = mp_level->getTileIdx( ( int )mouse_pos.x, ( int )mouse_pos.y );

            if( startIdx != targetIdx && Tile::EMPTY == targetTile )
            {
                m_vPath = mp_pathFinder->getShortestPath( startIdx, targetIdx );

                if( !m_vPath.empty() )
                {
                    m_state = State::MOVING;
                    m_pathIdx = 0;
                }
            }
        }
    }
}

Vec2 Unit::calcDirection()
{
    const Vec2 nextTileCenter = mp_level->getTileCenter( m_vPath[ m_pathIdx ] );

    Vec2 dist = nextTileCenter - m_pos;
    if( fabsf( dist.x ) < 1.5f && fabsf( dist.y ) < 1.5f )
    {
        m_pos_tile.x = ( float )( m_vPath[ m_pathIdx ] % mp_level->getWidth() );
        m_pos_tile.y = ( float )( m_vPath[ m_pathIdx ] / mp_level->getWidth() );
        m_pathIdx++;        
    }
    if( m_pathIdx == m_vPath.size() )
    {
        m_state = State::STANDING;
    }

    m_dir.x = nextTileCenter.x - m_pos.x;
    m_dir.y = nextTileCenter.y - m_pos.y;

    m_dir.Normalize();

    if( m_dir.x > 0.1f && m_dir.y > 0.1f )
    {
        m_spriteDirection = Direction::DOWN_RIGHT;
    }
    else if( m_dir.x > 0.1f && m_dir.y < -0.1f )
    {
        m_spriteDirection = Direction::UP_RIGHT;
    }
    else if( m_dir.x > 0.1f && fabsf( m_dir.y ) < 0.1f )
    {
        m_spriteDirection = Direction::RIGHT;
    }
    else if( m_dir.x < -0.1f && m_dir.y < -0.1f )
    {
        m_spriteDirection = Direction::UP_LEFT;
    }
    else if( m_dir.x < -0.1f && m_dir.y > 0.1f )
    {
        m_spriteDirection = Direction::DOWN_LEFT;
    }
    else if( m_dir.x < -0.1f && fabsf( m_dir.y ) < 0.1f )
    {
        m_spriteDirection = Direction::LEFT;
    }
    else if( fabsf( m_dir.x ) < 0.1f && m_dir.y < -0.1f )
    {
        m_spriteDirection = Direction::UP;
    }
    else if( fabsf( m_dir.x ) < 0.1f && m_dir.y > 0.1f )
    {
        m_spriteDirection = Direction::DOWN;
    }
    return m_dir;
}

