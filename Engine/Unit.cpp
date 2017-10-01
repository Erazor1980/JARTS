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
    
    //m_pos.x     = pos_tile.x * m_size + m_size / 2 - 1;
    //m_pos.y     = pos_tile.y * m_size + m_size / 2 - 1;
    m_location.x    = pos_tile.x * m_size + m_size / 2 - 1;
    m_location.y    = pos_tile.y * m_size + m_size / 2 - 1;
    m_maxSpeed = 200;
    m_maxForce = 0.2f;
    m_velocity = { 0, 0 };
    m_acceleration = { 0, 0 };

    m_halfSize = m_size / 2;
    m_bb = RectF( m_location - Vec2( ( float )m_halfSize, ( float )m_halfSize ), m_location + Vec2( ( float )m_halfSize + 1, ( float )m_halfSize + 1 ) );

    m_spriteDirection = ( Direction )( rand() % 8 );
}

void Unit::draw( Graphics& gfx, const bool drawPath ) const
{
    /* drawing current (remaining) path when selected */
    if( drawPath && m_bSelected && State::MOVING == m_state )
    {
        //mp_level->drawPath( gfx, m_vPath, m_pathIdx );
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
    /*if( State::MOVING == m_state )
    {
        calcDirection();

        m_vel = m_dir * m_speed;
        m_pos += m_vel * dt;
        m_bb = RectF( m_pos - Vec2( ( float )m_halfSize, ( float )m_halfSize ), m_pos + Vec2( ( float )m_halfSize + 1, ( float )m_halfSize + 1 ) );
    }*/

    if( State::MOVING == m_state )
    {
        followPath( vUnits, m_path, dt );

        calcSpriteDirection();

        m_velocity += m_acceleration;
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

#if 0 // old
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
            const int startIdx = ( int )m_pos_tile.y * mp_level->getWidth() + ( int )m_pos_tile.x;
            const int targetIdx = mp_level->getTileIdx( ( int )mouse_pos.x, ( int )mouse_pos.y );

            if( startIdx == targetIdx )
            {
                return;
            }
            if( m_type == UnitType::JET )
            {
                m_vPath.clear();
                m_vPath.push_back( targetIdx );
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
                    m_vPath = mp_pathFinder->getShortestPath( startIdx, targetIdx );

                    if( !m_vPath.empty() )
                    {
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
}
#endif
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
            const int startIdx = mp_level->getTileIdx( m_location ); //( int )m_pos_tile.y * mp_level->getWidth() + ( int )m_pos_tile.x;
            const int targetIdx = mp_level->getTileIdx( ( int )mouse_pos.x, ( int )mouse_pos.y );

            if( startIdx == targetIdx )
            {
                return;
            }
            if( m_type == UnitType::JET )
            {
                /*m_vPath.clear();
                m_vPath.push_back( targetIdx );*/
                std::vector< Vec2 > vPoints = { m_location, mp_level->getTileCenter( targetIdx ) };
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
                    //m_vPath = mp_pathFinder->getShortestPath( startIdx, targetIdx );

                    m_path = mp_pathFinder->calcShortestPath( startIdx, targetIdx );

                    //if( !m_vPath.empty() )
                    //{
                        m_state = State::MOVING;
                        m_pathIdx = 0;
#if !_DEBUG
                        m_soundCommand.Play();
#endif
                    //}
                }
            }
        }
    }
}

#if 0
void Unit::calcDirection()
{
    if( m_type == UnitType::JET )
    {
        const Vec2 targetTileCenter = mp_level->getTileCenter( m_vPath[ m_vPath.size() - 1 ] );
        
        m_dir = targetTileCenter - m_pos;
        if( fabsf( m_dir.x ) < 1.5f && fabsf( m_dir.y ) < 1.5f )
        {
            m_pathIdx = ( int )m_vPath.size();
        }
    }
    else
    {
        const Vec2 nextTileCenter = mp_level->getTileCenter( m_vPath[ m_pathIdx ] );

        Vec2 dist = nextTileCenter - m_pos;
        if( fabsf( dist.x ) < 1.5f && fabsf( dist.y ) < 1.5f )
        {
            m_pos_tile.x = ( float )( m_vPath[ m_pathIdx ] % mp_level->getWidth() );
            m_pos_tile.y = ( float )( m_vPath[ m_pathIdx ] / mp_level->getWidth() );
            m_pathIdx++;
        }

        m_dir.x = nextTileCenter.x - m_pos.x;
        m_dir.y = nextTileCenter.y - m_pos.y;
    }
    
    if( m_pathIdx == m_vPath.size() )
    {
        m_state = State::STANDING;
    }
    
    m_dir.Normalize();

    if( m_dir.x > 0.4f && m_dir.y > 0.4f )
    {
        m_spriteDirection = Direction::DOWN_RIGHT;
    }
    else if( m_dir.x > 0.4f && m_dir.y < -0.4f )
    {
        m_spriteDirection = Direction::UP_RIGHT;
    }
    else if( m_dir.x > 0.4f && fabsf( m_dir.y ) < 0.4f )
    {
        m_spriteDirection = Direction::RIGHT;
    }
    else if( m_dir.x < -0.4f && m_dir.y < -0.4f )
    {
        m_spriteDirection = Direction::UP_LEFT;
    }
    else if( m_dir.x < -0.4f && m_dir.y > 0.4f )
    {
        m_spriteDirection = Direction::DOWN_LEFT;
    }
    else if( m_dir.x < -0.4f && fabsf( m_dir.y ) < 0.4f )
    {
        m_spriteDirection = Direction::LEFT;
    }
    else if( fabsf( m_dir.x ) < 0.4f && m_dir.y < -0.4f )
    {
        m_spriteDirection = Direction::UP;
    }
    else if( fabsf( m_dir.x ) < 0.4f && m_dir.y > 0.4f )
    {
        m_spriteDirection = Direction::DOWN;
    }
}
#endif

void Unit::calcSpriteDirection()
{
    if( m_velocity.x > 0.4f && m_velocity.y > 0.4f )
    {
        m_spriteDirection = Direction::DOWN_RIGHT;
    }
    else if( m_velocity.x > 0.4f && m_velocity.y < -0.4f )
    {
        m_spriteDirection = Direction::UP_RIGHT;
    }
    else if( m_velocity.x > 0.4f && fabsf( m_velocity.y ) < 0.4f )
    {
        m_spriteDirection = Direction::RIGHT;
    }
    else if( m_velocity.x < -0.4f && m_velocity.y < -0.4f )
    {
        m_spriteDirection = Direction::UP_LEFT;
    }
    else if( m_velocity.x < -0.4f && m_velocity.y > 0.4f )
    {
        m_spriteDirection = Direction::DOWN_LEFT;
    }
    else if( m_velocity.x < -0.4f && fabsf( m_velocity.y ) < 0.4f )
    {
        m_spriteDirection = Direction::LEFT;
    }
    else if( fabsf( m_velocity.x ) < 0.4f && m_velocity.y < -0.4f )
    {
        m_spriteDirection = Direction::UP;
    }
    else if( fabsf( m_velocity.x ) < 0.4f && m_velocity.y > 0.4f )
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
        //update( dt );
        return;
    }
    else if( m_pathIdx == path.getWayPoints().size() - 1 )
    {
        seek( path.getWayPoints().back(), dt );
        //update( dt );

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

Vec2 Unit::seek( const Vec2 & target, const float dt )
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
    return steer;
}
Vec2 Unit::separateFromOtherUnits( const std::vector< Unit >& vUnits, const float dt )
{
    //TODO add the size of the vehicle here!
    //float r;
    float desiredseparation = m_size; // r * 2;

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
    // Step 1: Predict the vehicle�s future location.
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
