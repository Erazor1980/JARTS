#include "Entity.h"

Entity::Entity()
{
    m_pos = { 55, 55 };
}

void Entity::draw( Graphics& gfx ) const
{
    gfx.DrawCircle( m_pos.x, m_pos.y, 7, Colors::Blue );
}
