#include "Entity.h"
#include <assert.h>

Entity::Entity( const Vec2 pos_tile, const Level* const pLevel, PathFinder* const pPathFinder )
    :
    mp_level( pLevel ),
    mp_pathFinder( pPathFinder )
{
    assert( pLevel->isInitialized() );
    assert( pos_tile.x >= 0 && pos_tile.x < pLevel->getWidth()
            && pos_tile.y >= 0 && pos_tile.y < pLevel->getHeight() );

    const int tSize = pLevel->getTileSize();
    m_pos_tile  = pos_tile;
    m_pos.x     = pos_tile.x * tSize + tSize / 2 - 1;
    m_pos.y     = pos_tile.y * tSize + tSize / 2 - 1;

    m_halfSize = m_size / 2.0f;
    m_bb = RectF( m_pos - Vec2( m_halfSize, m_halfSize ), m_pos + Vec2( m_halfSize + 1, m_halfSize + 1 ) );
}

void Entity::draw( Graphics& gfx ) const
{
    if( m_bSelected )
    {
        gfx.DrawCircle( ( int )m_pos.x, ( int )m_pos.y, 16, Colors::Blue );
    }
    gfx.DrawCircle( ( int )m_pos.x, ( int )m_pos.y, 12, Colors::Black );

#if DEBUG_INFOS
    /* draw bounding box */
    gfx.DrawRectBorder( m_bb, 1, Colors::White );
#endif

    /* drawing current path when selected, will be removed later */
    if( m_bSelected && State::MOVING == m_state )
    {
        mp_level->drawPath( gfx, m_vPath );
    }
}

void Entity::update( const Mouse::Event::Type& type, const Vec2& mouse_pos, const bool shift_pressed, const float dt )
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
        m_bb = RectF( m_pos - Vec2( m_halfSize, m_halfSize ), m_pos + Vec2( m_halfSize + 1, m_halfSize + 1 ) );
    }
}

void Entity::select()
{
    m_bSelected = true;
}
void Entity::deselect()
{
    m_bSelected = false;
}

void Entity::handleMouse( const Mouse::Event::Type& type, const Vec2& mouse_pos, const bool shift_pressed )
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

            if( Tile::EMPTY == targetTile )
            {
                const int startIdx = ( int )m_pos_tile.y * mp_level->getWidth() + ( int )m_pos_tile.x; 
                const int targetIdx = mp_level->getTileIdx( ( int )mouse_pos.x, ( int )mouse_pos.y );
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

Vec2 Entity::calcDirection()
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
    
    return m_dir;
}

