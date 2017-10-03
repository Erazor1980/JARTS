#include "Unit.h"
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>

Unit::Unit( const Vei2 pos_tile,
            const Team team,
            const Level& level,
            PathFinder& pathFinder,
            std::vector< Unit* >& vpUnits,
            const UnitType type,
            const Surface& unitSprite,
            std::vector< Sound >& vSoundEffects )
    :
    m_level( level ),
    m_pathFinder( pathFinder ),
    m_vpUnits( vpUnits ),
    m_unitSprite( unitSprite ),
    m_vSoundEffects( vSoundEffects )
{
    assert( level.isInitialized() );
    assert( pos_tile.x >= 0 && pos_tile.x < level.getWidth()
            && pos_tile.y >= 0 && pos_tile.y < level.getHeight() );

    m_type      = type;
    m_size      = unitSprite.GetHeight();

    /* calculating rectangles for unit sprite steps (directions) */
    for( int i = 0; i < 8; ++i )
    {
        m_vSpriteRects.emplace_back( i * m_size, ( i + 1 ) * m_size, 0, m_size );
    }
    
    m_location.x    = pos_tile.x * m_size + m_size / 2.0f - 1;
    m_location.y    = pos_tile.y * m_size + m_size / 2.0f - 1;
    m_tileIdx       = m_level.getTileIdx( m_location );
    m_team          = team;

    m_bIsGroundUnit = true;
    if( UnitType::TANK == type )
    {
        m_maxSpeed              = 100;
        m_maxForce              = 0.3f;
        m_life                  = 200;
        m_attackRadius          = 115;
        m_attackDamage          = 20;
        m_timeBetweenAttacks    = 1000;
    }
    else if( UnitType::JET == type )
    {
        m_maxSpeed              = 250;
        m_maxForce              = 0.15f;
        m_bIsGroundUnit         = false;
        m_life                  = 100;
        m_attackRadius          = 150;
        m_attackDamage          = 10;
        m_timeBetweenAttacks    = 300;
    }
    else if( UnitType::SOLDIER == type )
    {
        m_maxSpeed              = 45;
        m_maxForce              = 0.5f;
        m_life                  = 50;
        m_attackRadius          = 70;
        m_attackDamage          = 5;
        m_timeBetweenAttacks    = 200;
    }
    m_maxLife           = m_life;
    m_oneThirdMaxLife   = m_maxLife / 3.0f;

    m_halfSize = m_size / 2;
    m_bb = RectF( m_location - Vec2( ( float )m_halfSize, ( float )m_halfSize ), m_location + Vec2( ( float )m_halfSize + 1, ( float )m_halfSize + 1 ) );


    /* create random start direction */
    m_velocity.x = -50.0f + rand() % 100;
    m_velocity.y = -50.0f + rand() % 100;
    calcSpriteDirection();

    /* reset all movement values */
    m_velocity          = { 0, 0 };
    m_acceleration      = { 0, 0 };
    m_velocity.x        = 0;
    m_velocity.y        = 0;
}

void Unit::draw( Graphics& gfx, const bool drawExtraInfos ) const
{
    drawLifeBar( gfx );

    /* drawing extra infos, like current path or attackRadius, when selected */
    if( drawExtraInfos && m_bSelected )
    {
        if( State::MOVING == m_state )
        {
            m_path.draw( gfx );
        }

        if( m_state == State::ATTACKING )
        {
            gfx.DrawCircleBorder( m_location, ( int )m_attackRadius, Colors::Red );
        }
        else
        {
            gfx.DrawCircleBorder( m_location, ( int )m_attackRadius, Colors::White );
        }
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
                    m_unitSprite, Colors::White );

    if( UnitType::TANK == m_type )
    {
        drawCannon( gfx );
    }

#if DEBUG_INFOS
    /* draw bounding box */
    gfx.DrawRectBorder( m_bb, 1, Colors::White );
#endif
}
void Unit::update( const float dt )
{
    float distToEnemy = 0.0f;
    if( mp_currentEnemy )
    {
        distToEnemy = ( mp_currentEnemy->getLocation() - m_location ).GetLength();
        if( m_targetIdx != mp_currentEnemy->getTileIdx() )
        {
            m_targetIdx = mp_currentEnemy->getTileIdx();
            if( distToEnemy > m_attackRadius )
            {
                recalculatePath();
            }
        }

        if( UnitType::TANK == m_type )
        {
            Vec2 dir = mp_currentEnemy->getLocation() - m_location;
            m_cannonOrientation = atan2( dir.y, dir.x );
        }

        if( mp_currentEnemy->isDestroyed() )
        {
            m_state = State::STANDING;
        }
    }

    if( State::MOVING == m_state )
    {
        if( UnitType::JET == m_type )
        {
            applyForce( separateFromOtherUnits( dt ) * 1.5f );
            applyForce( seek( m_path.getWayPoints().back(), dt, true ) );
            float d = ( m_location - m_path.getWayPoints().back() ).GetLength();
            if( d < m_distToTile / 2 )
            {
                stop();
            }
        }
        else
        {
            followPath( dt );
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

        m_tileIdx = m_level.getTileIdx( m_location );

        if( mp_currentEnemy )
        {
            float distToEnemy = ( mp_currentEnemy->getLocation() - m_location ).GetLength();
            if( distToEnemy <= m_attackRadius )
            {
                m_state = State::ATTACKING;
            }
        }
    }
    else if( State::WAITING == m_state )
    {
        if( m_currWaitingTime < m_waitingTimeMAX )
        {
            if( !recalculatePath() )
            {
                m_currWaitingTime += dt;
                m_state = State::WAITING;
            }
        }
        else
        {
            stop();
        }
    }
    else if( State::ATTACKING == m_state )
    {
        if( distToEnemy <= m_attackRadius )
        {
            std::chrono::steady_clock::time_point currTime = std::chrono::steady_clock::now();
            long long timeElapsed = std::chrono::duration_cast< std::chrono::milliseconds >( currTime - m_timeLastShot ).count();
            if( timeElapsed > m_timeBetweenAttacks )
            {
                m_timeLastShot = currTime;
                shoot();
            }
        }
        else
        {
            m_state = State::MOVING;
        }
    }
}

void Unit::shoot()
{
#if !_DEBUG
    m_vSoundEffects[ ( int )SoundOrder::ATTACK ].Play();
#endif
    mp_currentEnemy->takeDamage( m_attackDamage, m_type );
}
void Unit::handleMouse( const Mouse::Event::Type& type, const Vec2& mouse_pos, const bool shift_pressed )
{
#if !_DEBUG  /* in debug mode we can select and give commands to enemies */
    if( Team::_A != m_team )
    {
        return;
    }
#endif

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
            Tile targetTile     = m_level.getTileType( ( int )mouse_pos.x, ( int )mouse_pos.y );
            const int startIdx  = m_level.getTileIdx( m_location );
            m_targetIdx         = m_level.getTileIdx( ( int )mouse_pos.x, ( int )mouse_pos.y );

            if( startIdx == m_targetIdx )
            {
                return;
            }

            /* check if target is an enemy */
            mp_currentEnemy = nullptr;
            for( int i = 0; i < m_vpUnits.size(); ++i )
            {
                if( m_vpUnits[ i ]->getTeam() == m_team )
                {
                    continue;
                }
                if( m_targetIdx == m_vpUnits[ i ]->getTileIdx() )
                {
                    mp_currentEnemy = m_vpUnits[ i ];
                    break;
                }
            }
            if( mp_currentEnemy )
            {
                float distToEnemy = ( mp_currentEnemy->getLocation() - m_location ).GetLength();
                if( distToEnemy <= m_attackRadius )
                {
                    m_state = State::ATTACKING;
                    return;
                }
            }

            if( m_type == UnitType::JET )
            {
                /* for jets adding only startIdx and targetIdx to path -> they are targeting directly this tile (not the path segment) */
                if( isTileOccupied( m_targetIdx ) )
                {
                    m_targetIdx = findNextFreeTile( m_targetIdx );
                }
                std::vector< Vec2 > vPoints = { m_location, m_level.getTileCenter( m_targetIdx ) };
                m_path = Path( vPoints );
                m_state = State::MOVING;
                m_pathIdx = 0;
#if !_DEBUG
                m_vSoundEffects[ ( int )SoundOrder::COMMAND ].Play( 1, 0.5f );
#endif
            }
            else
            {
                if( Tile::EMPTY == targetTile )
                {
                    m_path = m_pathFinder.calcShortestPath( startIdx, m_targetIdx, std::vector< int >() );

                    m_state = State::MOVING;
                    m_pathIdx = 0;
#if !_DEBUG
                    m_vSoundEffects[ ( int )SoundOrder::COMMAND ].Play( 1, 0.5f );
#endif
                }
            }
        }
    }
}
void Unit::drawCannon( Graphics& gfx ) const
{
    const int x     = ( int )m_location.x;
    const int y     = ( int )m_location.y;
    const int newX  = x + ( int )( 0.7f * m_size * cos( m_cannonOrientation ) );
    const int newY  = y + ( int )( 0.7f * m_size * sin( m_cannonOrientation ) );

    Color colorGun = { 115, 115, 115 };
    Color colorGun2 = { 75, 75, 75 };
    gfx.DrawLine( x, y, newX, newY, colorGun );
    gfx.DrawLine( x + 1, y, newX + 1, newY, colorGun );
    gfx.DrawLine( x + 2, y, newX + 2, newY, colorGun );
    gfx.DrawLine( x - 1, y, newX - 1, newY, colorGun );
    gfx.DrawLine( x - 2, y, newX - 2, newY, colorGun );
    gfx.DrawLine( x, y + 1, newX, newY + 1, colorGun );
    gfx.DrawLine( x, y + 2, newX, newY + 2, colorGun );
    gfx.DrawLine( x, y - 1, newX, newY - 1, colorGun );
    gfx.DrawLine( x, y - 2, newX, newY - 2, colorGun );
    gfx.DrawCircle( ( x + newX ) / 2, ( y + newY ) / 2, 4, colorGun2 );
    gfx.DrawCircle( newX, newY, 2, colorGun2 );
}
void Unit::calcSpriteDirection()
{
    if( UnitType::TANK == m_type && m_velocity.GetLength() )
    {
        if( mp_currentEnemy )                   /* cannon direction to the target enemy */
        {
            Vec2 dir = mp_currentEnemy->getLocation() - m_location;
            m_cannonOrientation = atan2( dir.y, dir.x );
        }
        else if( State::MOVING == m_state )     /* cannon direction to the target tile */
        {
            Vec2 dir = m_level.getTileCenter( m_targetIdx ) - m_location;
            m_cannonOrientation = atan2( dir.y, dir.x );
        }
        else
        {
            m_cannonOrientation = atan2( m_velocity.y, m_velocity.x );
        }
    }

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
void Unit::drawLifeBar( Graphics& gfx ) const
{
    Color lifebarColor;
    const int x = ( int )m_location.x;
    const int y = ( int )m_location.y;

    float lifeMaxLifeRatio = ( float )m_life / m_maxLife;

    if( m_life >= m_oneThirdMaxLife + m_oneThirdMaxLife )
    {
        lifebarColor = Colors::Green;
    }
    else
    {
        if( m_life >= m_oneThirdMaxLife )
        {
            lifebarColor = Colors::Yellow;
        }
        else
        {
            lifebarColor = Colors::Red;
        }
    }

    int barPosY = y - ( int )( 1.3f * m_halfSize );
    gfx.DrawLine( x - m_halfSize + ( int )( ( 1 - lifeMaxLifeRatio ) * m_size ), barPosY + 1, x + m_halfSize, barPosY + 1, lifebarColor );
    gfx.DrawLine( x - m_halfSize + ( int )( ( 1 - lifeMaxLifeRatio ) * m_size ), barPosY, x + m_halfSize, barPosY, lifebarColor );
    gfx.DrawLine( x - m_halfSize + ( int )( ( 1 - lifeMaxLifeRatio ) * m_size ), barPosY - 1, x + m_halfSize, barPosY - 1, lifebarColor );
    gfx.DrawLine( x - m_halfSize, barPosY - 2, x + m_halfSize, barPosY - 2, Colors::White );		// top border   -----
    gfx.DrawLine( x - m_halfSize, barPosY - 2, x - m_halfSize, barPosY + 2, Colors::White );		// left border  |_____
    gfx.DrawLine( x + m_halfSize, barPosY - 2, x + m_halfSize, barPosY + 2, Colors::White );		// right border _____|
    gfx.DrawLine( x - m_halfSize, barPosY + 2, x + m_halfSize, barPosY + 2, Colors::White );		// bottom border _____
}
void Unit::stop()
{
    m_state             = State::STANDING;
    m_acceleration      = { 0, 0 };
    m_velocity          = { 0, 0 };
    m_pathIdx           = 0;
    m_currWaitingTime   = 0;
}
void Unit::followPath( const float dt )
{
    //Vec2 separateResult = separateFromOtherUnits( vUnits, dt );
    //applyForce( separateResult * 1.5f );

    if( m_path.getWayPoints().empty() )
    {
        return;
    }
    else if( m_pathIdx == m_path.getWayPoints().size() - 1 )
    {
        seek( m_path.getWayPoints().back(), dt );
        float d = ( m_path.getWayPoints().back() - m_location ).GetLength();

        if( d < m_distToTile )
        {
            stop();
        }
        return;
    }
    else if( m_pathIdx == m_path.getWayPoints().size() - 2 )
    {
        if( isTileOccupied( getNextTileIdx() ) )     /* move back to own tile center if next one is occupied */
        {
            seek( m_level.getTileCenter( m_tileIdx ), dt );
            float d = ( m_level.getTileCenter( m_tileIdx ) - m_location ).GetLength();

            if( d < m_distToTile )
            {
                stop();
            }
            return;
        }
    }
    else
    {
        if( isTileOccupied( getNextTileIdx() ) )
        {
            if( !recalculatePath() )
            {
                return;
            }
        }
    }

#if 1   // test with next tile center as target (not line segment point)
    if( m_path.getWayPoints().size() > m_pathIdx + 1 )
    {
        Vec2 end = m_path.getWayPoints()[ m_pathIdx + 1 ];
        applyForce( seek( end, dt ) );

        float d = ( end - m_location ).GetLength();
        if( d < m_distToTile )
        {
            m_pathIdx++;
        }
    }
#else
    Vec2 start = m_path.getWayPoints()[ m_pathIdx ];
    Vec2 end = m_path.getWayPoints()[ m_pathIdx + 1 ];
    followLineSegment( start, end, m_path.getRadius(), dt );
#endif
}
Vec2 Unit::seek( const Vec2& target, const float dt, const bool enableBreaking )
{
    Vec2 desired = target - m_location;
    float distToTarget = desired.GetLength();
    desired.Normalize();

    float startToBreak = 20;     /* pixels to target */
    if( UnitType::JET == m_type )
    {
        startToBreak = m_maxSpeed / 5;
    }

    if( enableBreaking && distToTarget < startToBreak )
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
Vec2 Unit::separateFromOtherUnits( const float dt )
{
    float desiredSeparation = ( float )m_halfSize * 1.5f;

    Vec2 sum( 0, 0 );
    int count = 0;

    for( auto other : m_vpUnits )
    {
        if( other->isGroundUnit() != m_bIsGroundUnit )
        {
            continue;
        }
        float d = ( m_location - other->getLocation() ).GetLength();
        if( ( d > 0 ) && ( d < desiredSeparation ) )
        {
            Vec2 diff = m_location - other->getLocation();
            diff.Normalize();

            // What is the magnitude of the PVector pointing away from the other vehicle? The closer it is, the more we should flee.
            // The farther, the less. So we divide by the distance to weight it appropriately.
            diff *=  ( 1.0f / d );
            sum += diff;
            count++;
        }
    }

    Vec2 steer = { 0, 0 };
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
    Vec2 seekResult = { 0, 0 };
    if( distance > radius )
    {
        seekResult = seek( target, dt );
    }

    applyForce( seekResult );
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
void Unit::handleSelectionRect( const RectI& selectionRect )
{
    if( m_team != Team::_A )
    {
        return;
    }
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
    m_vSoundEffects[ ( int )SoundOrder::SELECTION ].Play();
#endif
}
void Unit::deselect()
{
    m_bSelected = false;
}
void Unit::takeDamage( const int damage, const UnitType EnemyType )
{
    //TODO add damage taken depending on own type and enemy type

    m_life -= damage;
    m_life = std::max( m_life, 0 );

    if( m_life == 0 )
    {
#if !_DEBUG
        m_vSoundEffects[ ( int )SoundOrder::DEATH ].Play();
#endif
    }
}
std::vector< int > Unit::checkNeighbourhood()
{
    std::vector< int > vOccupiedNeighbourTiles;
    const int width     = m_level.getWidth();
    const int height    = m_level.getHeight();

    const int currX     = m_tileIdx % width;
    const int currY     = m_tileIdx / width;

    for( int x = -1; x < 2; ++x )
    {
        for( int y = -1; y < 2; ++y )
        {
            if( x == 0 && y == 0 )
            {
                continue;   // center idx itself
            }
            int tmpX = currX + x;
            int tmpY = currY + y;

            if( tmpX >= 0 && tmpX < width && tmpY >= 0 && tmpY < height )
            {
                int idx = tmpY * width + tmpX;
                for( const auto& u : m_vpUnits )
                {
                    if( u->isGroundUnit() != m_bIsGroundUnit )
                    {
                        continue;
                    }
                    if( idx == u->getTileIdx() || idx == u->getNextTileIdx() )
                    {
                        vOccupiedNeighbourTiles.push_back( idx );
                    }
                }
            }
        }
    }

    return vOccupiedNeighbourTiles;
}
int Unit::findNextFreeTile( const int targetIdx )
{
    std::vector< int > vFreeNeighbourTiles;
    const int width     = m_level.getWidth();
    const int height    = m_level.getHeight();

    const int currX     = targetIdx % width;
    const int currY     = targetIdx / width;

    for( int x = -1; x < 2; ++x )
    {
        for( int y = -1; y < 2; ++y )
        {
            if( x == 0 && y == 0 )
            {
                continue;   // center idx itself
            }
            int tmpX = currX + x;
            int tmpY = currY + y;

            if( tmpX >= 0 && tmpX < width && tmpY >= 0 && tmpY < height )
            {
                int idx = tmpY * width + tmpX;
                if( m_bIsGroundUnit && Tile::OBSTACLE == m_level.getTileType( idx ) )     /* skip obstacle tiles if unit is ground unit */
                {
                    continue;
                }
                bool occupied = false;
                for( const auto& u : m_vpUnits )
                {
                    if( u->isGroundUnit() != m_bIsGroundUnit )
                    {
                        continue;
                    }
                    if( idx == u->getTileIdx() || idx == u->getNextTileIdx() )
                    {
                        occupied = true;
                        break;
                    }
                }
                if( !occupied )
                {
                    vFreeNeighbourTiles.push_back( idx );
                }
            }
        }
    }

    const int nFreeTiles = ( int )vFreeNeighbourTiles.size();
    if( vFreeNeighbourTiles.empty() )
    {
        return -1;
    }
    else
    {
        return vFreeNeighbourTiles[ rand() % nFreeTiles ];
    }
}
bool Unit::isTileOccupied( const int idx )
{
    for( const auto& u : m_vpUnits )
    {
        if( u->isGroundUnit() != m_bIsGroundUnit )
        {
            continue;
        }
        float d = ( u->getLocation() - m_location ).GetLengthSq();   /* to test if we are not comparing with ourselves */
        if( d > 0 )
        {
            if( u->getTileIdx() == idx || u->getNextTileIdx() == idx )
            {
                return true;
            }
        }
    }
    return false;
}
bool Unit::recalculatePath()
{
    std::vector< int > vOccupiedIdx = checkNeighbourhood();
    m_path                          = m_pathFinder.calcShortestPath( m_tileIdx, m_targetIdx, vOccupiedIdx );
    m_pathIdx                       = 0;

    if( m_path.getWayPoints().size() == 1 )
    {
        m_state = State::WAITING;
        return false;
    }
    else
    {
        m_state = State::MOVING;
    }
    return true;
}
