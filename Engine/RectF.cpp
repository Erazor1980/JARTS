#include "RectF.h"
#include <algorithm>

RectF::RectF( float left_in, float right_in, float top_in, float bottom_in )
    :
    left( left_in ),
    right( right_in ),
    top( top_in ),
    bottom( bottom_in )
{
}

RectF::RectF( const Vec2& topLeft, const Vec2& bottomRight )
    :
    RectF( topLeft.x, bottomRight.x, topLeft.y, bottomRight.y )
{
}

RectF::RectF( const Vec2& topLeft, float width, float height )
    :
    RectF( topLeft, topLeft + Vec2( width, height ) )
{
}

bool RectF::IsOverlappingWith( const RectF& other ) const
{
    return right > other.left && left < other.right
        && top < other.bottom && bottom > other.top;
}

bool RectF::IsContainedBy( const RectF & other ) const
{
    return left >= other.left && right <= other.right &&
        top >= other.top && bottom <= other.bottom;
}

bool RectF::Contains( const Vei2& point )
{
    Normalize();
    return point.x >= left && point.x < right && point.y >= top && point.y < bottom;
}

bool RectF::Contains( const Vec2& point )
{
    Normalize();
    return point.x >= left && point.x < right && point.y >= top && point.y < bottom;
}

RectF RectF::FromCenter( const Vec2& center, float halfWidth, float halfHeight )
{
    const Vec2 half( halfWidth, halfHeight );
    return RectF( center - half, center + half );
}

RectF RectF::GetExpanded( float offset ) const
{
    return RectF( left - offset, right + offset, top - offset, bottom + offset );
}

Vec2 RectF::GetCenter() const
{
    return Vec2( ( left + right ) / 2.0f, ( top + bottom ) / 2.0f );
}

void RectF::Normalize()
{
    if( left > right )
    {
        std::swap( left, right );
    }
    if( bottom < top )
    {
        std::swap( bottom, top );
    }
}
