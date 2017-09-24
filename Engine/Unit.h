#pragma once
#include "Graphics.h"
#include "Sound.h"
#include "Mouse.h"
#include "Level.h"
#include "PathFinding.h"

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

    void update( const float dt );
    void update( const Mouse::Event::Type& type, const Vec2& mouse_pos, const bool shift_pressed, const float dt );
    void handleSelectionRect( const RectI& selectionRect );
    void select();
    void deselect();
    void recalculatePath( const std::vector< int >& vOccupiedNeighbourTiles );

    Vec2 getPosition() const
    {
        return m_pos;
    }
    RectF getBoundigBox() const
    {
        return m_bb;
    }
    UnitType getType() const
    {
        return m_type;
    }
    bool isGroundUnit() const
    {
        return m_bGroundUnit;
    }
    int getPosTileIdx() const
    {
        return ( int )m_pos_tile.y * mp_level->getWidth() + ( int )m_pos_tile.x;;
    }
    int getNextTileIdx() const;     /* return -1 if not moving */

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
    void calcDirection();   /* calculated direction depending on current and next tile while moving */

    /* position in level in pixel coordinates - for smooth movement and bounding box */
    Vec2 m_pos;

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

    /* Movement */
    Vec2 m_vel = { 0.0f, 0.0f };
    float m_speed = 110.0f;
    Vec2 m_dir = { 0.0f, 0.0f };

    /* Attributes */
    UnitType m_type;
    int m_life;
    bool m_bGroundUnit;
};