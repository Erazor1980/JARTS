#pragma once
#include "Graphics.h"
#include "Sound.h"
#include "Mouse.h"
#include "Level.h"
#include "PathFinding.h"
#include "Path.h"
#include "Defines.h"

enum class UnitType
{
    TANK = 0,
    SOLDIER,
    JET
};

class Unit
{
public:
    Unit( const Vec2 pos_tile, 
          const Level* const pLevel, 
          PathFinder* const pPathFinder, 
          const UnitType type,
          const std::vector< Surface >& vUnitSprites,
          Sound& soundSelect,
          Sound& soundCommand );

    void draw( Graphics& gfx, const bool drawPath = false ) const;

    void update( const std::vector< Unit >& vUnits, const float dt );
    void update( const std::vector< Unit >& vUnits, 
                 const Mouse::Event::Type& type,
                 const Vec2& mouse_pos,
                 const bool shift_pressed,
                 const float dt );
    void handleSelectionRect( const RectI& selectionRect );
    void select();
    void deselect();
    Vec2 getLocation() const
    {
        return m_location;
    }
    //Vec2 getPosition() const
    //{
    //    return m_pos;
    //}
    RectF getBoundigBox() const
    {
        return m_bb;
    }

    enum class State
    {
        STANDING,
        MOVING,
        ATTACKING
    };

    enum class Direction    /* for the 8 sprite directions */
    {
        UP = 0,
        UP_RIGHT,
        RIGHT,
        DOWN_RIGHT,
        DOWN,
        DOWN_LEFT,
        LEFT,
        UP_LEFT
    };
private:
    void handleMouse( const Mouse::Event::Type& type, const Vec2& mouse_pos, const bool shift_pressed );
    void calcSpriteDirection();     /* which sprite to choose depending on current direction */

    /* position in level in tiles - for path planning */
    Vec2 m_pos_tile;

    /* bounding box (for selection in first place) */
    RectF m_bb;
    int m_size; // width/height of bb in pixels
    int m_halfSize;

    /* true when selected by the player and ready for receiving commands */
    bool m_bSelected = false;

    State m_state = State::STANDING;

    /* graphics & sounds */
    const std::vector< Surface >& m_vUnitSprites;   /* vector with sprites for all unit types, index = UnitType */
    std::vector< RectI > m_vSpriteRects;            /* rectangles for single steps (direction) of a unit sprite set */
    Direction m_spriteDirection;
    Sound& m_soundSelect;
    Sound& m_soundCommand;
    bool m_bInsideSelectionRect = false;

    /* pointer to the current level */
    const Level* const mp_level;

    /* path planning */
    PathFinder* const mp_pathFinder;
    std::vector< int > m_vPath;
    int m_pathIdx = -1;                 /* current idx from path */

    /* Attributes */
    UnitType m_type;
    int m_life;


    /* NEW PATH FOLLOWING */
    Vec2 m_location;
    Vec2 m_acceleration;
    Vec2 m_velocity;

    float m_maxForce;
    float m_maxSpeed;
    //int m_pathIdx = 0;

    Path m_path;

    void followPath( const std::vector< Unit >& vUnits, const Path& path, const float dt );
    void followLineSegment( const Vec2& start, const Vec2& end, const float radius, const float dt );
    Vec2 seek( const Vec2& target, const float dt );
    Vec2 separateFromOtherUnits( const std::vector< Unit >& vUnits, const float dt );
    void applyForce( const Vec2& force );
    bool isNormalPointValid( const Vec2 & start, const Vec2 & end, const Vec2& normalPoint );
    Vec2 getNormalPoint( const Vec2& p, const Vec2& a, const Vec2& b );
};