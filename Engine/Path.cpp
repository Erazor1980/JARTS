#include "Path.h"

Path::Path( const float radius )
{
    m_radius = radius;
}

Path::Path( const std::vector<Vec2>& vPoints, const float radius )
{
    m_vWayPoints = vPoints;
    m_radius = radius;
}

void Path::addPoint( const Vec2& newPoint )
{
    m_vWayPoints.push_back( newPoint );
}

void Path::draw( Graphics& gfx ) const
{
    for( int i = 1; i < m_vWayPoints.size(); ++i )
    {
        gfx.DrawLine( m_vWayPoints[ i - 1 ], m_vWayPoints[ i ], Colors::Yellow );
        gfx.DrawCircle( m_vWayPoints[ i ], ( int )m_radius, Colors::Gray );
    }
}
