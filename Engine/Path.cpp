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

void Path::draw( Graphics& gfx, const Vei2& camPos ) const
{
    if( m_vWayPoints.empty() )
    {
        return;
    }

    const Vei2 halfScreen( Graphics::halfScreenWidth, Graphics::halfScreenHeight );
    const Vei2 offset = camPos - halfScreen;

    gfx.DrawCircle( Vei2( ( int )m_vWayPoints.front().x, ( int )m_vWayPoints.front().y ) - offset, ( int )m_radius, Colors::Gray );
    for( int i = 1; i < m_vWayPoints.size(); ++i )
    {
        const Vei2 p1 = Vei2( ( int )m_vWayPoints[ i - 1 ].x, ( int )m_vWayPoints[ i - 1 ].y ) - offset;
        const Vei2 p2 = Vei2( ( int )m_vWayPoints[ i ].x, ( int )m_vWayPoints[ i ].y ) - offset;
        gfx.DrawLine( p1, p2, Colors::Yellow );
        gfx.DrawCircle( p2, ( int )m_radius, Colors::Gray );
    }
}
