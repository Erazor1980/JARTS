#include "Entity.h"
#include "PathFinding.h"
#include <assert.h>

Entity::Entity( const Vec2 pos_tile, const Level* const pLevel )
    :
    mp_level( pLevel )
{
    assert( pLevel->isInitialized() );
    assert( pos_tile.x >= 0 && pos_tile.x < pLevel->getWidth()
            && pos_tile.y >= 0 && pos_tile.y < pLevel->getHeight() );

    const float tSize = pLevel->getTileSize();
    m_pos_tile  = pos_tile;
    m_pos.x     = pos_tile.x * tSize + tSize / 2 - 1;
    m_pos.y     = pos_tile.y * tSize + tSize / 2 - 1;

    float halfSize = m_size / 2.0f;
    m_bb = RectF( m_pos - Vec2( halfSize, halfSize ), m_pos + Vec2( halfSize + 1, halfSize + 1 ) );
}

void Entity::draw( Graphics& gfx ) const
{
    if( m_bSelected )
    {
        gfx.DrawCircle( ( int )m_pos.x, ( int )m_pos.y, 10, Colors::Blue );
    }
    gfx.DrawCircle( ( int )m_pos.x, ( int )m_pos.y, 7, Colors::Gray );

#if DEBUG_INFOS
    /* draw bounding box */
    gfx.DrawRectBorder( m_bb, 1, Colors::White );
#endif
}

void Entity::update( const Mouse::Event::Type& type, const Vec2& mouse_pos, const bool shift_pressed )
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
}

void Entity::select()
{
    m_bSelected = true;
}

void Entity::deselect()
{
    m_bSelected = false;
}
