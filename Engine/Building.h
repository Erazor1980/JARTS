#pragma once
#include "Vei2.h"

class Building
{
public:
    enum class Type
    {
        BARRACKS,
        FACTORY
    };

public:
    Building( Building::Type type );

    Vei2 getSizeInTiles() const
    {
        return m_sizeInTiles;
    }
    static Vei2 getSizeInTiles( const Type type );
    
private:
    Vei2 m_sizeInTiles = { 1, 1 };     /* number of tiles in x- and y-direction the building occupies */
    Type m_type;
};