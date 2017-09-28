#include "Vehicle.h"

Vehicle::Vehicle( const float x, const float y )
{
    m_location.x = x;
    m_location.y = y;

    m_maxSpeed = 100;
    m_maxForce = 0.2f;
}

void Vehicle::update( const float dt )
{
    m_velocity += m_acceleration;
    if( m_velocity.GetLength() > m_maxSpeed * dt )
    {
        m_velocity.Normalize();
        m_velocity *= m_maxSpeed * dt;
    }

    m_location += m_velocity;
    m_acceleration = { 0, 0 };
}

void Vehicle::moveToTarget( const Vec2& target, const float dt )
{
    if( target.GetLength() > 0 )
    {
        seek( target, dt );
    }

    update( dt );
}

void Vehicle::follow( const Vec2& start, const Vec2& end, const float radius, const float dt )
{
    // Step 1: Predict the vehicle’s future location.
    Vec2 predict = m_velocity;
    predict.Normalize();
    predict *= 25;          /* 25 pixels away (maybe add parameter or dynamic value depending on speed etc later) */
    Vec2 predictLoc = m_location + predict;

    // Step 2: Find the normal point along the path.
    Vec2 normalPoint = getNormalPoint( predictLoc, start, end );

    // Step 3: Move a little further along the path and set a target.
    Vec2 dir = end - start;
    dir.Normalize();
    dir *= 10;              /* 10 pixels away (maybe add parameter or dynamic value depending on speed etc later) */
    Vec2 target = normalPoint + dir;
    
    // Step 4: If we are off the path, seek that target in order to stay on the path.
    float distance = ( normalPoint - predictLoc ).GetLength();
    if( distance > radius )
    {
        seek( target, dt );
    }

    update( dt );
}

#if 0 // old version
void Vehicle::followPath( const Path& path, const float dt )
{
    Vec2 start, end;

    // Step 1: Predict the vehicle’s future location.
    Vec2 predict = m_velocity;
    predict.Normalize();
    predict *= 25;          /* 25 pixels away (maybe add parameter or dynamic value depending on speed etc later) */
    Vec2 predictLoc = m_location + predict;

    // Step 2: Find the normal point along the path.
    float worldRecord = 1000000;
    Vec2 target;
    for( int i = 0; i < path.getWayPoints().size() - 1; i++ )
    {
        Vec2 a = path.getWayPoints()[ i ];
        Vec2 b = path.getWayPoints()[ i + 1 ];
        Vec2 normalPoint = getNormalPoint( predictLoc, a, b );

        //TODO change here for all paths (not only left to right!)
        if( normalPoint.x < a.x || normalPoint.x > b.x )
        {
            // Use the end point of the segment as our normal point if we can’t find one.
            normalPoint = b;
        }

        float distance = ( normalPoint - predictLoc ).GetLength();


        //If we beat the record, then this should be our target!

        if( distance < worldRecord )
        {
            worldRecord = distance;
            target = normalPoint;

            start = a;
            end = b;
        }
    }

    // Step 3: Move a little further along the path and set a target.
    Vec2 dir = end - start;
    dir.Normalize();
    dir *= 10;              /* 10 pixels away (maybe add parameter or dynamic value depending on speed etc later) */
    Vec2 targetShifted = target + dir;

    // Step 4: If we are off the path, seek that target in order to stay on the path.
    float distance = ( target - predictLoc ).GetLength();
    if( distance > path.getRadius() )
    {
        seek( target, dt );
    }

    update( dt );
}
#endif

void Vehicle::followPath( const Path& path, const float dt )
{
    if( path.getWayPoints().empty() )
    {
        update( dt );
        return;
    }
    else if( m_pathIdx == path.getWayPoints().size() - 1 )
    {
        seek( path.getWayPoints().back(), dt );
        update( dt );
        return;
    }

  
    Vec2 start = path.getWayPoints()[ m_pathIdx ];
    Vec2 end = path.getWayPoints()[ m_pathIdx + 1 ];

    follow( start, end, path.getRadius(), dt );

    float d = ( end - m_location ).GetLength();
    if( d < 10 )
    {
        m_pathIdx++;
    }
}
void Vehicle::draw( Graphics& gfx )
{
    gfx.DrawCircle( m_location, 10, Colors::Green );

    Vec2 v = m_velocity.GetNormalized();
    v *= 15;

    gfx.DrawLine( m_location, m_location + v, Colors::White );
}

void Vehicle::applyForce( const Vec2& force )
{
    m_acceleration += force;
}

void Vehicle::seek( const Vec2& target, const float dt )
{
    Vec2 desired = target - m_location;
    float distToTarget = desired.GetLength();
    desired.Normalize();
    
    const float startToBreak = 20;     /* pixels to target */
    if( distToTarget < startToBreak )
    {
        desired *= ( distToTarget / startToBreak ) * m_maxSpeed * dt;

    }
    else
    {
        desired *= m_maxSpeed * dt;
    }

    Vec2 steer = desired - m_velocity;
    if( steer.GetLength() > m_maxForce )
    {
        steer.Normalize();
        steer *= m_maxForce;
    }

    applyForce( steer );
}