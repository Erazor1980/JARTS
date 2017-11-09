#include "Building.h"
#include "SpriteEffect.h"

Building::Building( Building::Type type, const Surface& image, const std::vector< int >& vTileIndices, const Level& level )
    :
    m_image( image ),
    m_vTileIndices( vTileIndices ),
    m_level( level )
{
    m_type          = type;
    m_sizeInTiles   = getSizeInTiles( type );
}

void Building::draw( Graphics& gfx, const Vei2& camPos, const bool drawExtraInfos ) const
{
    const Vei2 halfScreen( Graphics::halfScreenWidth, Graphics::halfScreenHeight );
    const Vei2 offset = camPos - halfScreen;

    /* drawing extra infos, like current path or attackRadius */
    if( drawExtraInfos )
    {
        //add stuff if needed
    }

    /* top left point of the building in screen coordinates */
    Vec2 tl = m_level.getTileCenter( m_vTileIndices.front() ) - Vec2( m_level.getTileSize() / 2.0f, m_level.getTileSize() / 2.0f );
    tl -= offset;
    gfx.DrawSprite( ( int )tl.x, ( int )tl.y, m_image, SpriteEffect::Chroma( { 255, 242, 0 } ) );
}

Vei2 Building::getSizeInTiles( const Type type )
{
    switch( type )
    {
    case Type::BARRACKS:
        return Vei2( 2, 2 );
        break;
    case Type::FACTORY:
        return Vei2( 3, 2 );
        break;
    default:
        return Vei2( 0, 0 );
    }    
}
