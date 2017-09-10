#include "Entity.h"

Entity::Entity( const Vec2 pos )
{
    m_pos = pos;

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

void Entity::update( const Mouse::Event::Type& type, const Vec2& mouse_pos )
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
            m_bSelected = false;
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
