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
    m_soundSelect.Play();
}
void Unit::deselect()
{
    m_bSelected = false;
}

void Unit::update( const float dt )
{
    if( State::MOVING == m_state )
    {
        calcDirection();

        m_vel = m_dir * m_speed;
        m_pos += m_vel * dt;
        m_bb = RectF( m_pos - Vec2( ( float )m_halfSize, ( float )m_halfSize ), m_pos + Vec2( ( float )m_halfSize + 1, ( float )m_halfSize + 1 ) );
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

                m_soundCommand.Play();
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

                        m_soundCommand.Play();
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
                return;
            }
        }
    }
    else
    {
        ///* check if tile is occupied by another unit */
        //for( int i = 0; i < mp_level->getGroundUnitsPositions().size(); ++i )
        //{
        //    const int neighbourUnitIdx = mp_level->getGroundUnitsPositions()[ i ];
        //    if( m_vPath[ m_pathIdx ] == neighbourUnitIdx )
        //    {
        //        if( m_pathIdx == m_vPath.size() - 1 )   /* target tile occupied, but end of path -> standing */
        //        {
        //            m_state = State::STANDING;
        //            return;
        //        }
        //        else        /* find another path to target */
        //        {
        //            m_vPath = mp_pathFinder->getShortestPath( m_vPath[ m_pathIdx - 1 ], m_vPath[ m_vPath.size() - 1 ], neighbourUnitIdx );

        //            if( !m_vPath.empty() )
        //            {
        //                m_pathIdx = 0;
        //                return;
        //            }
        //            else
        //            {
        //                m_state = State::STANDING;
        //                return;
        //            }
        //        }
        //    }
        //}

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
                return;
            }

            ///* check if tile is occupied by another unit */
            //for( int i = 0; i < mp_level->getGroundUnitsPositions().size(); ++i )
            //{
            //    const int neighbourUnitIdx = mp_level->getGroundUnitsPositions()[ i ];
            //    if( m_vPath[ m_pathIdx ] == neighbourUnitIdx )
            //    {
            //        if( m_pathIdx == m_vPath.size() - 1 )   /* target tile occupied, but end of path -> standing */
            //        {
            //            m_state = State::STANDING;
            //            return;
            //        }
            //        else        /* find another path to target */
            //        {
            //            m_vPath = mp_pathFinder->getShortestPath( m_vPath[ m_pathIdx - 1 ], m_vPath[ m_vPath.size() - 1 ], neighbourUnitIdx );

            //            if( !m_vPath.empty() )
            //            {
            //                m_pathIdx = 0;
            //                return;
            //            }
            //            else
            //            {
            //                m_state = State::STANDING;
            //                return;
            //            }
            //        }
            //    }
            //}
        }

        m_dir.x = nextTileCenter.x - m_pos.x;
        m_dir.y = nextTileCenter.y - m_pos.y;
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

