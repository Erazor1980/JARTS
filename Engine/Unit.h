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
    enum class State
    {
        STANDING,
        WAITING,            /* if path is temporary blocked */
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
public:
    Unit( const Vei2 pos_tile, 
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
    bool isGroundUnit() const
    {
        return m_bIsGroundUnit;
    }
    Vec2 getLocation() const
    {
        return m_location;
    }
    Vei2 getLocationInt() const
    {
        return Vei2( ( int )m_location.x, ( int )m_location.y );
    }
    Vec2 getVelocity() const
    {
        return m_velocity;
    }
    RectF getBoundigBox() const
    {
        return m_bb;
    }
    State getState() const
    {
        return m_state;
    }
    int getTileIdx() const
    {
        return m_tileIdx;
    }
    int getNextTileIdx() const
    {
        if( State::MOVING == m_state )
        {
            if( m_pathIdx + 1 < m_path.getWayPoints().size() )
            {
                return mp_level->getTileIdx( m_path.getWayPoints()[ m_pathIdx + 1 ] );
            }
        }
        return -1;
    }
    float getWaitingTime() const
    {
        return m_currWaitingTime;
    }
private:
    void handleMouse( const Mouse::Event::Type& type, const Vec2& mouse_pos, const bool shift_pressed );
    void calcSpriteDirection();     /* which sprite to choose depending on current direction */
    void stop();

    /* position in level in tiles - for path planning */
    int m_tileIdx;
    int m_targetIdx = -1;

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
    //std::vector< int > m_vPath;
    int m_pathIdx = -1;                             /* current idx from path */
    static constexpr float m_distToTile = 10.0f;    /* tile reached if unit's distance to tile's center is lower */
    const float m_waitingTimeMAX = 1.0f;            /* how long to wait to find a free path in seconds */
    float m_currWaitingTime = 0.0f;                 /* curran waiting time in seconds */

    /* Attributes */
    UnitType m_type;
    bool m_bIsGroundUnit;
    int m_life;


    /* NEW PATH FOLLOWING */
    Vec2 m_location;
    Vec2 m_acceleration;
    Vec2 m_velocity;

    float m_maxForce;
    float m_maxSpeed;
    //int m_pathIdx = 0;

    Path m_path;

    std::vector< int > checkNeighbourhood( const std::vector< Unit >& vUnits );     /* returns indeces of occupied tiles (other units or their next targets) */
    bool checkTile( const int idx, const std::vector< Unit >& vUnits );             /* check if a tile is occupied by other units or their next targets */
    bool recalculatePath( const std::vector< Unit >& vUnits );                      /* returns false if path is temporary blocked */
    void followPath( const std::vector< Unit >& vUnits, const float dt );
    void followLineSegment( const Vec2& start, const Vec2& end, const float radius, const float dt );
    Vec2 seek( const Vec2& target, const float dt, const bool enableBreaking = false );
    Vec2 separateFromOtherUnits( const std::vector< Unit >& vUnits, const float dt );
    void separateFromOtherUnitsNew( const std::vector< Unit >& vUnits, const float dt );
    void applyForce( const Vec2& force );
    bool isNormalPointValid( const Vec2 & start, const Vec2 & end, const Vec2& normalPoint );
    Vec2 getNormalPoint( const Vec2& p, const Vec2& a, const Vec2& b );    
};