#pragma once
#include "Vec2.h"
#include "Graphics.h"
#include <vector>

class Path
{
public:
    Path( const float radius = 5 );
    Path( const std::vector< Vec2 >& vPoints, const float radius = 5 );

    void addPoint( const Vec2& newPoint );
    void addPoint( float x, float y )
    {
        addPoint( Vec2( x, y ) );
    }
    void draw( Graphics& gfx ) const;

    float getRadius() const
    {
        return m_radius;
    }
    std::vector< Vec2 > getWayPoints() const
    {
        return m_vWayPoints;
    }
private:
    std::vector< Vec2 > m_vWayPoints;       /* way points of the path (tile centers) */
    float m_radius;                         /* thickness of the path in pixels */
};