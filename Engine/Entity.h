#pragma once
#include "Graphics.h"
#include "Mouse.h"

#define DEBUG_INFOS false

class Entity
{
public:
    Entity( const Vec2 pos );

    void draw( Graphics& gfx ) const;

    void update( const Mouse::Event::Type& type, const Vec2& mouse_pos );
    void select();
    void deselect();
private:
    /* position in level in pixel coordinates */
    Vec2 m_pos;

    /* bounding box (for selection in first place) */
    RectF m_bb;
    float m_size = 16; // width/height of bb in pixels

    /* true when selected by the player and ready for receiving commands */
    bool m_bSelected = false;
};