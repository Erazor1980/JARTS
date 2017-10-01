#include "Unit.h"
#include <assert.h>

Unit::Unit( const Vec2 pos_tile,
            const Level* const pLevel,
            PathFinder* const pPathFinder,
            const UnitType type,
            const std::vector< Surface >& vUnitSprites,
            Sound& soundSelect,
            Sound& soundCommand )
    :
    mp_level( pLevel ),
    mp_pathFinder( pPathFinder ),
    m_vUnitSprites( vUnitSprites ),
    m_soundSelect( soundSelect ),
    m_soundCommand( soundCommand )
{
    assert( pLevel->isInitialized() );
    assert( pos_tile.x >= 0 && pos_tile.x < pLevel->getWidth()
            && pos_tile.y >= 0 && pos_tile.y < pLevel->getHeight() );
    assert( vUnitSprites.size() > 0 );

    m_type      = type;
    m_size      = m_vUnitSprites[ ( int )m_type ].GetHeight();
    m_pos_tile  = pos_tile;

    /* calculating rectangles for unit sprite steps (directions) */
    for( int i = 0; i < 8; ++i )
    {
        m_vSpriteRects.emplace_back( i * m_size, ( i + 1 ) * m_size, 0, m_size );
    }
    
    m_location.x    = pos_tile.x * m_size + m_size / 2 - 1;
    m_location.y    = pos_tile.y * m_size + m_size / 2 - 1;
    if( UnitType::TANK == type )
    {
        m_maxSpeed = 100;
        m_maxForce = 0.3f;
    }
    else if( UnitType::JET == type )
    {
        m_maxSpeed = 250;
        m_maxForce = 0.2f;
    }
    m_velocity = { 0, 0 };
    m_acceleration = { 0, 0 };

    m_halfSize = m_size / 2;
    m_bb = RectF( m_location - Vec2( ( float )m_halfSize, ( float )m_halfSize ), m_location + Vec2( ( float )m_halfSize + 1, ( float )m_halfSize + 1 ) );

    m_velocity.x = ( float )1 + rand() % 10;
    m_velocity.y = ( float )1 + rand() % 10;
    m_velocity.Normalize();
    calcSpriteDirection();
}

void Unit::draw( Graphics& gfx, const bool drawPath ) const
{
    /* drawing current (remaining) path when selected */
    if( drawPath && m_bSelected && State::MOVING == m_state )
    {
        m_path.draw( gfx );
    }

    if( m_bSelected )
    {
        gfx.DrawRectCorners( m_bb, Colors::Green );
    }
    else if( m_bInsideSelectionRect )
    {
        gfx.DrawRectCorners( m_bb, Colors::White );
    }

    gfx.DrawSprite( ( int )m_location.x - m_halfSize, ( int )m_location.y - m_halfSize, m_vSpriteRects[ ( int )m_spriteDirection ],
                    m_vUnitSprites[ ( int )m_type ], Colors::White );

#if DEBUG_INFOS
    /* draw bounding box */
    gfx.DrawRectBorder( m_bb, 1, Colors::White );
#endif
}

void Unit::update( const std::vector< Unit >& vUnits, 
                   const Mouse::Event::Type& type,
                   const Vec2& mouse_pos,
                   const bool shift_pressed,
                   const float dt )
{
    if( mp_level->getLevelRect().Contains( mouse_pos ) )
    {
        handleMouse( type, mouse_pos, shift_pressed );
    }

    update( vUnits, dt );
}

void Unit::handleSelectionRect( const RectI& selectionRect )
{
    RectI r = selectionRect.getNormalized();
    if( r.Contains( m_location ) )
    {
        m_bInsideSelectionRect = true;
    }
    else
    {
        m_bInsideSelectionRect = false;
    }
}

void Unit::select()
{
    m_bSelected = true;
#if !_DEBUG
    m_soundSelect.Play();
#endif
}
void Unit::deselect()
{
    m_bSelected = false;
}

void Unit::update( const std::vector< Unit >& vUnits, const float dt )
{
    if( State::MOVING == m_state )
    {
        if( UnitType::JET == m_type )
        {
            applyForce( seek( m_path.getWayPoints().front(), dt ) );
        }
        else
        {
            followPath( vUnits, m_path, dt );
        }

        m_velocity += m_acceleration;
        calcSpriteDirection();
        if( m_velocity.GetLength() > m_maxSpeed * dt )
        {
            m_velocity.Normalize();
            m_velocity *= m_maxSpeed * dt;
        }

        m_location += m_velocity;
        m_bb = RectF( m_location - Vec2( ( float )m_halfSize, ( float )m_halfSize ),
                      m_location + Vec2( ( float )m_halfSize + 1, ( float )m_halfSize + 1 ) );
        m_acceleration = { 0, 0 };
    }
}

void Unit::handleMouse( const Mouse::Event::Type& type, const Vec2& mouse_pos, const bool shift_pressed )
{
    if( type == Mouse::Event::Type::LPress )
    {
        if( !m_bSelected )
        {
            /* check if we click inside the bounding box */
            if( m_bb.IsOverlappingWith( RectF( mouse_pos, 1, 1 ) ) )
            {
                select();
            }
        }
        else
        {
            if( !shift_pressed )
            {
                deselect();
            }
        }
    }
    else if( type == Mouse::Event::Type::RPress )
    {
        if( m_bSelected )
        {
            Tile targetTile = mp_level->getTileType( ( int )mouse_pos.x, ( int )mouse_pos.y );
            const int startIdx = mp_level->getTileIdx( m_location );
            const int targetIdx = mp_level->getTileIdx( ( int )mouse_pos.x, ( int )mouse_pos.y );

            if( startIdx == targetIdx )
            {
                return;
            }
            if( m_type == UnitType::JET )
            {
                /* for jets adding only targetIdx to path -> they are targeting directly this tile (not the path segment) */
                std::vector< Vec2 > vPoints = { mp_level->getTileCenter( targetIdx ) };
                m_path = Path( vPoints );
                m_state = State::MOVING;
                m_pathIdx = 0;
#if !_DEBUG
                m_soundCommand.Play();
#endif
            }
            else
            {
                if( Tile::EMPTY == targetTile )
                {
                    m_path = mp_pathFinder->calcShortestPath( startIdx, targetIdx );

                    m_state = State::MOVING;
                    m_pathIdx = 0;
#if !_DEBUG
                    m_soundCommand.Play();
#endif
                }
            }
        }
    }
}

void Unit::calcSpriteDirection()
{
    Vec2 velNorm = m_velocity.GetNormalized();

    if( velNorm.x > 0.4f && velNorm.y > 0.4f )
    {
        m_spriteDirection = Direction::DOWN_RIGHT;
    }
    else if( velNorm.x > 0.4f && velNorm.y < -0.4f )
    {
        m_spriteDirection = Direction::UP_RIGHT;
    }
    else if( velNorm.x > 0.4f && fabsf( velNorm.y ) < 0.4f )
    {
        m_spriteDirection = Direction::RIGHT;
    }
    else if( velNorm.x < -0.4f && velNorm.y < -0.4f )
    {
        m_spriteDirection = Direction::UP_LEFT;
    }
    else if( velNorm.x < -0.4f && velNorm.y > 0.4f )
    {
        m_spriteDirection = Direction::DOWN_LEFT;
    }
    else if( velNorm.x < -0.4f && fabsf( velNorm.y ) < 0.4f )
    {
        m_spriteDirection = Direction::LEFT;
    }
    else if( fabsf( velNorm.x ) < 0.4f && velNorm.y < -0.4f )
    {
        m_spriteDirection = Direction::UP;
    }
    else if( fabsf( velNorm.x ) < 0.4f && velNorm.y > 0.4f )
    {
        m_spriteDirection = Direction::DOWN;
    }
}

void Unit::followPath( const std::vector< Unit >& vUnits, const Path& path, const float dt )
{
    Vec2 separateResult = separateFromOtherUnits( vUnits, dt );
    applyForce( separateResult * 1.5f );

    if( path.getWayPoints().empty() )
    {
        return;
    }
    else if( m_pathIdx == path.getWayPoints().size() - 1 )
    {
        seek( path.getWayPoints().back(), dt );

        float d = ( path.getWayPoints().back() - m_location ).GetLength();
        if( d < 10 )
        {
            m_pathIdx = 0;
            m_state = State::STANDING;
        }
        return;
    }
    
    Vec2 start = path.getWayPoints()[ m_pathIdx ];
    Vec2 end = path.getWayPoints()[ m_pathIdx + 1 ];

    followLineSegment( start, end, path.getRadius(), dt );

    float d = ( end - m_location ).GetLength();
    if( d < 10 )
    {
        m_pathIdx++;
    }
}

Vec2 Unit::seek( const Vec2& target, const float dt )
{
    Vec2 desired = target - m_location;
    float distToTarget = desired.GetLength();
    desired.Normalize();

    float startToBreak = 20;     /* pixels to target */
    if( UnitType::JET == m_type )
    {
        startToBreak = m_maxSpeed / 5;
    }

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
    return steer;
}
Vec2 Unit::separateFromOtherUnits( const std::vector< Unit >& vUnits, const float dt )
{
    //TODO add the size of the vehicle here!
    //float r;
    float desiredseparation = ( float )m_size; // r * 2;

    Vec2 sum( 0, 0 );
    int count = 0;

    //TODO separate between land and air units!
    for( auto other : vUnits )
    {
        float d = ( m_location - other.getLocation() ).GetLength();
        if( ( d > 0 ) && ( d < desiredseparation ) )
        {
            Vec2 diff = m_location - other.getLocation();
            diff.Normalize();

            // What is the magnitude of the PVector pointing away from the other vehicle? The closer it is, the more we should flee.
            // The farther, the less. So we divide by the distance to weight it appropriately.
            diff *=  ( 1.0f / d );
            sum += diff;
            count++;
        }
    }

    Vec2 steer ={ 0, 0 };
    if( count > 0 )
    {
        sum *= ( 1.0f / count );
        sum.Normalize();
        sum *= m_maxSpeed * dt;
        steer = sum - m_velocity;
        if( steer.GetLength() > m_maxForce )
        {
            steer.Normalize();
            steer *= m_maxForce;
        }
    }
    return steer;
}
void Unit::followLineSegment( const Vec2& start, const Vec2& end, const float radius, const float dt )
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

    //m_normal = normalPoint;

    // Step 3: Move a little further along the path and set a target.
    Vec2 dir = end - start;
    dir.Normalize();
    dir *= 10;              /* 10 pixels away (maybe add parameter or dynamic value depending on speed etc later) */
    Vec2 target = normalPoint + dir;

    //m_target = target;

    // Step 4: If we are off the path, seek that target in order to stay on the path.
    float distance = ( normalPoint - predictLoc ).GetLength();
    Vec2 seekResult ={ 0, 0 };
    if( distance > radius )
    {
        seekResult = seek( target, dt );
    }

    applyForce( seekResult );
    //update( dt );
}
bool Unit::isNormalPointValid( const Vec2 & start, const Vec2 & end, const Vec2& normalPoint )
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
void Unit::applyForce( const Vec2& force )
{
    m_acceleration += force;
}
Vec2 Unit::getNormalPoint( const Vec2& p, const Vec2& a, const Vec2& b )
{
    //vector that points from a to p
    Vec2 ap = p - a;

    //vector that points from a to b
    Vec2 ab = b - a;

    // Using the dot product for scalar projection
    ab.Normalize();
    ab *= ap.DotProduct( ab );

    // Finding the normal point along the line segment
    Vec2 normalPoint = a + ab;

    return normalPoint;
}
