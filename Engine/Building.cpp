#include "Building.h"

Building::Building( Building::Type type )
{
    m_type          = type;
    m_sizeInTiles   = getSizeInTiles( type );
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
