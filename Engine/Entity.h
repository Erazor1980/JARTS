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

    void update( const Mouse::Event::Type& type, const Vec2& mouse_pos, const bool shift_pressed, const float dt );
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
    Vec2 calcDirection();   /* calculated direction depending on current and next tile while moving */

    /* position in level in pixel coordinates - for smooth movement and bounding box */
    Vec2 m_pos;

    /* position in level in tiles - for path planning */
    Vec2 m_pos_tile;

    /* bounding box (for selection in first place) */
    RectF m_bb;
    float m_size = 16; // width/height of bb in pixels
    float m_halfSize;

    /* true when selected by the player and ready for receiving commands */
    bool m_bSelected = false;

    State m_state = State::STANDING;

    /* pointer to the current level */
    const Level* const mp_level;

    /* path planning */
    PathFinder* const mp_pathFinder;
    std::vector< int > m_vPath;
    int m_pathIdx = -1;                 /* current idx from path */

    /* attributes */
    Vec2 m_vel = { 0.0f, 0.0f };
    float m_speed = 110.0f;
    Vec2 m_dir = { 0.0f, 0.0f };
};