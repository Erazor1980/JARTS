#include "Vehicle.h"
#include <algorithm>

Vehicle::Vehicle( const float x, const float y )
{
    m_location.x = x;
    m_location.y = y;

    m_maxSpeed = 200;
    m_maxForce = 0.3f;

    m_velocity = { 1, 1 };

#if DRAW_TRACKS
    m_trackColor = Color( rand() % 255, rand() % 255, rand() % 255 );
#endif
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

#if DRAW_TRACKS
    if( m_qTrack.size() > m_maxTrack )
    {
        m_qTrack.pop_front();
    }
    m_qTrack.push_back( m_location );
#endif
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
    
    // test if normal point lies on the segment, if not use the end point of the segment
    if( !isNormalPointValid( start, end, normalPoint ) )
    {
        normalPoint = end;
    }

    m_normal = normalPoint;

    // Step 3: Move a little further along the path and set a target.
    Vec2 dir = end - start;
    dir.Normalize();
    dir *= 10;              /* 10 pixels away (maybe add parameter or dynamic value depending on speed etc later) */
    Vec2 target = normalPoint + dir;
    
    m_target = target;

    // Step 4: If we are off the path, seek that target in order to stay on the path.
    float distance = ( normalPoint - predictLoc ).GetLength();
    if( distance > radius )
    {
        seek( target, dt );
    }

    update( dt );
}

void Vehicle::followPathOld( const Path& path, const float dt )
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

        // test if normal point lies on the segment, if not use the end point of the segment
        if( !isNormalPointValid( a, b, normalPoint ) )
        {
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

void Vehicle::separate( const std::vector< Vehicle >& vVehicles, const float dt )
{
    //TODO add the size of the vehicle here!
    float r = 25;
    float desiredseparation = r * 2;

    Vec2 sum( 0, 0 );
    int count = 0;
    for( Vehicle other : vVehicles )
    {
        float d = ( m_location - other.m_location ).GetLength();
        if( ( d > 0 ) && ( d < desiredseparation ) )
        {
            Vec2 diff = m_location - other.m_location;
            diff.Normalize();

            // What is the magnitude of the PVector pointing away from the other vehicle? The closer it is, the more we should flee.
            // The farther, the less. So we divide by the distance to weight it appropriately.

            diff *=  ( 1.0f / d );
            sum += diff;
            count++;
        }
    }
    if( count > 0 )
    {
        sum *= ( 1.0f / count );
        sum.Normalize();
        sum *= m_maxSpeed * dt;
        Vec2 steer = sum - m_velocity;
        if( steer.GetLength() > m_maxForce )
        {
            steer.Normalize();
            steer *= m_maxForce;
        }

        applyForce( steer );
    }
}



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

        //TODO test with non stop path (after last point reached, go to first again)
        float d = ( path.getWayPoints().back() - m_location ).GetLength();
        if( d < 10 )
        {
            m_pathIdx = 0;
        }
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
#if DRAW_TRACKS
    for( int i = 0; i < m_qTrack.size(); ++i )
    {
        gfx.DrawCircle( m_qTrack[ i ], 2, m_trackColor );
    }
#endif

    gfx.DrawCircle( m_location, 10, Colors::Green );

    Vec2 v = m_velocity.GetNormalized();
    v *= 15;

    gfx.DrawLine( m_location, m_location + v, Colors::White );


    gfx.DrawCircle( m_target, 3, Colors::Blue );
    gfx.DrawCircle( m_normal, 3, Colors::Green );
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

bool Vehicle::isNormalPointValid( const Vec2 & start, const Vec2 & end, const Vec2& normalPoint )
{
    if( start.x != end.x )
    {
        if( normalPoint.x < std::min( start.x, end.x ) || normalPoint.x > std::max( start.x, end.x ) )
        {
            return false;
        }
    }
    else
    {
        if( normalPoint.y < std::min( start.y, end.y ) || normalPoint.y > std::max( start.y, end.y ) )
        {
            return false;
        }
    }
    return true;
}
