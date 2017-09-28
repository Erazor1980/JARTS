#pragma once
#include "Vec2.h"
#include "Graphics.h"
#include "Path.h"

class Vehicle
{
public:
    Vehicle( const float x, const float y );
    void update( const float dt );
    void moveToTarget( const Vec2& target, const float dt );
    void follow( const Vec2& start, const Vec2& end, const float radius, const float dt );

    void followPath( const Path& path, const float dt );
    void followPathOld( const Path& path, const float dt );

    void draw( Graphics& gfx );
private:
    void applyForce( const Vec2& force );
    void seek( const Vec2& target, const float dt );

    bool isNormalPointValid( const Vec2& start, const Vec2& end, const Vec2& normalPoint );

    Vec2 m_location;
    Vec2 m_velocity;
    Vec2 m_acceleration;

    float m_maxForce;
    float m_maxSpeed;

    //TODO for debugging, make private later!
public:
    int m_pathIdx = 0;

    //TODO test, remove later!
    Vec2 m_target;
    Vec2 m_normal;
};