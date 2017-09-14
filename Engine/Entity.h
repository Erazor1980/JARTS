#pragma once
#include "Graphics.h"
#include "Mouse.h"
#include "Level.h"
#include "PathFinding.h"

#include "Defines.h"

class Entity
{
public:
    Entity( const Vec2 pos_tile, const Level* const pLevel, PathFinder* const pPathFinder );

    void draw( Graphics& gfx ) const;

    void update( const Mouse::Event::Type& type, const Vec2& mouse_pos, const bool shift_pressed );
    void select();
    void deselect();

    enum class State
    {
        STANDING,
        MOVING,
        ATTACKING
    };
private:

    void handleMouse( const Mouse::Event::Type& type, const Vec2& mouse_pos, const bool shift_pressed );

    /* position in level in pixel coordinates - for smooth movement and bounding box */
    Vec2 m_pos;

    /* position in level in tiles - for path planning */
    Vec2 m_pos_tile;

    /* bounding box (for selection in first place) */
    RectF m_bb;
    float m_size = 16; // width/height of bb in pixels

    /* true when selected by the player and ready for receiving commands */
    bool m_bSelected = false;

    State m_state = State::STANDING;

    /* pointer to the current level and pathPlaner */
    const Level* const mp_level;
    PathFinder* const mp_pathFinder;

    std::vector< int > m_vCurrentPath;
};