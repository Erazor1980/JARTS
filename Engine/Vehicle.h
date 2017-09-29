#pragma once
#include "Vec2.h"
#include "Graphics.h"
#include "Path.h"

#include <deque>

#define DRAW_TRACKS 0

class Vehicle
{
public:
    Vehicle( const float x, const float y );
    void update( const float dt );
    void moveToTarget( const std::vector< Vehicle >& vVehicles, const Vec2& target, const float dt );
    void follow( const Vec2& start, const Vec2& end, const float radius, const float dt );

    void followPath( const std::vector< Vehicle >& vVehicles, const Path& path, const float dt );
    void followPathOld( const std::vector< Vehicle >& vVehicles, const Path& path, const float dt );

    void draw( Graphics& gfx );
private:
    void applyForce( const Vec2& force );
    Vec2 seek( const Vec2& target, const float dt );
    Vec2 separate( const std::vector< Vehicle >& vVehicles, const float dt );
    void applyBehaviors( const std::vector< Vehicle >& vVehicles, const Vec2& target, const float dt );

    bool isNormalPointValid( const Vec2& start, const Vec2& end, const Vec2& normalPoint );

    Vec2 m_location;
    Vec2 m_acceleration;

    float m_maxForce;
    float m_maxSpeed;

    //TODO for debugging, make private later!
public:
    int m_pathIdx = 0;
    Vec2 m_velocity;

    //TODO test for debugging, remove later!
    Vec2 m_target;
    Vec2 m_normal;

#if DRAW_TRACKS
    std::deque< Vec2 > m_qTrack;
    Color m_trackColor;
    const int m_maxTrack = 200;
#endif
};