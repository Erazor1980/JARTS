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

    void draw( Graphics& gfx );
private:
    void applyForce( const Vec2& force );
    void seek( const Vec2& target, const float dt );

    Vec2 m_location;
    Vec2 m_velocity;
    Vec2 m_acceleration;

    float m_maxForce;
    float m_maxSpeed;
};