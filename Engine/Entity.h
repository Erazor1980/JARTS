#pragma once
#include "Graphics.h"
#include "Mouse.h"
#include "Level.h"

#include "Defines.h"

class Entity
{
public:
    Entity( const Vec2 pos_tile, const Level* const pLevel );

    void draw( Graphics& gfx ) const;

    void update( const Mouse::Event::Type& type, const Vec2& mouse_pos, const bool shift_pressed );
    void select();
    void deselect();
private:
    /* position in level in pixel coordinates - for smooth movement and bounding box */
    Vec2 m_pos;

    /* position in level in tiles - for path planning */
    Vec2 m_pos_tile;

    /* bounding box (for selection in first place) */
    RectF m_bb;
    float m_size = 16; // width/height of bb in pixels

    /* true when selected by the player and ready for receiving commands */
    bool m_bSelected = false;

    /* pointer to the current level */
    const Level* const mp_level;
};