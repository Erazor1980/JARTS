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
          const Level& level, 
          PathFinder& pPathFinder,
          std::vector< Unit >& vEnemies,
          const UnitType type,
          const Surface& unitSprite,
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

    bool isSelected() const
    {
        return m_bSelected;
    }
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
                return m_level.getTileIdx( m_path.getWayPoints()[ m_pathIdx + 1 ] );
            }
        }
        return -1;
    }
    float getWaitingTime() const
    {
        return m_currWaitingTime;
    }
private:    
    void stop();

    /////////////////
    //// GENERAL ////
    /////////////////
    /* current Level and PathFinder references */
    const Level& m_level;
    PathFinder& m_pathFinder;

    State m_state = State::STANDING;

    /* Attributes */
    UnitType m_type;
    bool m_bIsGroundUnit;
    int m_life;
    int m_maxLife;
    float m_oneThirdMaxLife;

    //////////////////
    //// FIGHTING ////
    //////////////////
    std::vector< Unit >& m_vEnemies;

    ///////////////////////////
    //// SELECTION & MOUSE ////
    ///////////////////////////
    void handleMouse( const Mouse::Event::Type& type, const Vec2& mouse_pos, const bool shift_pressed, const std::vector< Unit >& vUnits );
    RectF m_bb;                                     /* bounding box (for selection in first place) */
    int m_size;                                     /* width/height of bb in pixels */
    int m_halfSize;
    bool m_bSelected = false;                       /* true when selected by the player and ready for receiving commands */

    ///////////////////////////
    //// GRAPHICS & SOUNDS ////
    ///////////////////////////
    void calcSpriteDirection();                     /* which sprite to choose depending on current direction */
    void drawLifeBar( Graphics& gfx ) const;

    const Surface& m_unitSprite;                    /* unit sprite tiles */
    std::vector< RectI > m_vSpriteRects;            /* rectangles for single steps (direction) of a unit sprite set */
    Direction m_spriteDirection;
    Sound& m_soundSelect;
    Sound& m_soundCommand;
    bool m_bInsideSelectionRect = false;

    /////////////////////////////////////
    //// PATH PLANNING AND MOVEMENTS ////
    /////////////////////////////////////
    int m_pathIdx = -1;                             /* current idx from path */
    static constexpr float m_distToTile = 10.0f;    /* tile reached if unit's distance to tile's center is lower */
    const float m_waitingTimeMAX = 2.0f;            /* how long to wait to find a free path in seconds */
    float m_currWaitingTime = 0.0f;                 /* curran waiting time in seconds */

    int m_tileIdx;
    int m_targetIdx = -1;

    Vec2 m_location;
    Vec2 m_acceleration;
    Vec2 m_velocity;

    float m_maxForce;
    float m_maxSpeed;
    Path m_path;

    std::vector< int > checkNeighbourhood( const std::vector< Unit >& vUnits );     /* returns indeces of occupied tiles (other units or their next targets) */
    int findNextFreeTile( const int targetIdx, const std::vector< Unit >& vUnits ); /* returns index of next free tile (considering units and obstacles) */
    bool isTileOccupied( const int idx, const std::vector< Unit >& vUnits );        /* check if a tile is occupied by other units or their next targets */
    bool recalculatePath( const std::vector< Unit >& vUnits );                      /* returns false if path is temporary blocked */
    void followPath( const std::vector< Unit >& vUnits, const float dt );
    void followLineSegment( const Vec2& start, const Vec2& end, const float radius, const float dt );
    Vec2 seek( const Vec2& target, const float dt, const bool enableBreaking = false );
    Vec2 separateFromOtherUnits( const std::vector< Unit >& vUnits, const float dt );
    void applyForce( const Vec2& force );
    bool isNormalPointValid( const Vec2 & start, const Vec2 & end, const Vec2& normalPoint );
    Vec2 getNormalPoint( const Vec2& p, const Vec2& a, const Vec2& b );    
};