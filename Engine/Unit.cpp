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

    m_type          = type;
    m_size          = m_vUnitSprites[ ( int )m_type ].GetHeight();
    m_pos_tile      = pos_tile;

    m_bGroundUnit   = true;
    if( UnitType::JET == type )     //TODO modify it after adding more flying units!
    {
        m_bGroundUnit = false;
    }

    /* calculating rectangles for unit sprite steps (directions) */
    for( int i = 0; i < 8; ++i )
    {
        m_vSpriteRects.emplace_back( i * m_size, ( i + 1 ) * m_size, 0, m_size );
    }

    
    m_pos.x     = pos_tile.x * m_size + m_size / 2 - 1;
    m_pos.y     = pos_tile.y * m_size + m_size / 2 - 1;

    m_halfSize = m_size / 2;
    m_bb = RectF( m_pos - Vec2( ( float )m_halfSize, ( float )m_halfSize ), m_pos + Vec2( ( float )m_halfSize + 1, ( float )m_halfSize + 1 ) );

    m_spriteDirection = ( Direction )( rand() % 8 );
}

void Unit::draw( Graphics& gfx, const bool drawPath ) const
{
    /* drawing current (remaining) path when selected */
    if( drawPath && m_bSelected && State::MOVING == m_state )
    {
        mp_level->drawPath( gfx, m_vPath, m_pathIdx );
    }

    if( m_bSelected )
    {
        gfx.DrawRectCorners( m_bb, Colors::Green );
    }
    else if( m_bInsideSelectionRect )
    {
        gfx.DrawRectCorners( m_bb, Colors::White );
    }

    gfx.DrawSprite( ( int )m_pos.x - m_halfSize, ( int )m_pos.y - m_halfSize, m_vSpriteRects[ ( int )m_spriteDirection ],
                    m_vUnitSprites[ ( int )m_type ], Colors::White );


#if DEBUG_INFOS
    /* draw bounding box */
    gfx.DrawRectBorder( m_bb, 1, Colors::White );
#endif
}

void Unit::update( const Mouse::Event::Type& type, const Vec2& mouse_pos, const bool shift_pressed, const float dt )
{
    if( mp_level->getLevelRect().Contains( mouse_pos ) )
    {
        handleMouse( type, mouse_pos, shift_pressed );
    }

    update( dt );
}

void Unit::handleSelectionRect( const RectI& selectionRect )
{
    RectI r = selectionRect.getNormalized();
    if( r.Contains( m_pos ) )
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

void Unit::recalculatePath( const std::vector< int >& vOccupiedNeighbourTiles, const float dt )
{
    if( m_state == State::STANDING )
    {
        return;
    }

    const int startIdx = ( int )m_pos_tile.y * mp_level->getWidth() + ( int )m_pos_tile.x;
    //const int targetIdx = m_vPath[ m_vPath.size() - 1 ];

    m_vPath = mp_pathFinder->getShortestPath( startIdx, m_targetIdx, vOccupiedNeighbourTiles );

    m_pathIdx = 0;
    if( !m_vPath.empty() )
    {
        m_state = State::MOVING;
    }
    else
    {
        if( m_currWaitingTime < m_waitingTimeMAX )
        {
            m_currWaitingTime += dt;
            m_state = State::WAITING;
        }
        else
        {
            m_currWaitingTime = 0;
            m_state = State::STANDING;
        }
    }
}

int Unit::getNextTileIdx() const
{
    if( State::MOVING == m_state )
    {
        return m_vPath[ m_pathIdx ];
    }
    else
    {
        return -1;
    }
}

void Unit::update( const float dt )
{
    if( State::MOVING == m_state )
    {
        calcDirection();
        calcSpriteDirection();

        move( dt );
    }
    if( State::STANDING == m_state )//|| State::WAITING == m_state )
    {
        m_dir =  mp_level->getTileCenter( m_pos_tile ) - m_pos;
        if( fabsf( m_dir.x ) > 1.5f || fabsf( m_dir.y ) > 1.5f )
        {
            m_dir.Normalize();
            calcSpriteDirection();
            move( dt );
        }
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
            const int startIdx = ( int )m_pos_tile.y * mp_level->getWidth() + ( int )m_pos_tile.x;
            //const int targetIdx = mp_level->getTileIdx( ( int )mouse_pos.x, ( int )mouse_pos.y );
            m_targetIdx = mp_level->getTileIdx( ( int )mouse_pos.x, ( int )mouse_pos.y );

            if( startIdx == m_targetIdx )
            {
                return;
            }
            if( m_type == UnitType::JET )
            {
                m_vPath.clear();
                m_vPath.push_back( m_targetIdx );
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
                    m_vPath = mp_pathFinder->getShortestPath( startIdx, m_targetIdx, std::vector< int >() );

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

void Unit::calcDirection()
{
    if( m_type == UnitType::JET )
    {
        const Vec2 targetTileCenter = mp_level->getTileCenter( m_vPath[ m_vPath.size() - 1 ] );
        
        m_dir = targetTileCenter - m_pos;
        if( fabsf( m_dir.x ) < 1.5f && fabsf( m_dir.y ) < 1.5f )
        {
            m_pathIdx = ( int )m_vPath.size();

            if( m_pathIdx == m_vPath.size() )
            {
                m_state = State::STANDING;
                m_pos_tile.x = ( float )( m_vPath[ m_pathIdx - 1 ] % mp_level->getWidth() );
                m_pos_tile.y = ( float )( m_vPath[ m_pathIdx - 1 ] / mp_level->getWidth() );
                m_currWaitingTime = 0;
                return;
            }
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

            if( m_pathIdx == m_vPath.size() )
            {
                m_state = State::STANDING;
                m_currWaitingTime = 0;
                return;
            }
        }

        m_dir.x = nextTileCenter.x - m_pos.x;
        m_dir.y = nextTileCenter.y - m_pos.y;
    }
   
    m_dir.Normalize();
}

void Unit::calcSpriteDirection()
{
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

void Unit::move( const float dt )
{
    m_vel = m_dir * m_speed;
    m_pos += m_vel * dt;
    m_bb = RectF( m_pos - Vec2( ( float )m_halfSize, ( float )m_halfSize ), m_pos + Vec2( ( float )m_halfSize + 1, ( float )m_halfSize + 1 ) );
}

