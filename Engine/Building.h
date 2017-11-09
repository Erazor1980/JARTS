#pragma once
#include "Vei2.h"
#include "Surface.h"
#include "Graphics.h"
#include "Level.h"

#include <vector>

class Building
{
public:
    enum class Type
    {
        BARRACKS,
        FACTORY
    };

public:
    Building( Building::Type type, const Surface& image, const std::vector< int >& vTileIndices, const Level& level );

    void draw( Graphics& gfx, const Vei2& camPos, const bool drawExtraInfos = false ) const;

    Vei2 getSizeInTiles() const
    {
        return m_sizeInTiles;
    }
    static Vei2 getSizeInTiles( const Type type );
    
    std::vector< int > getTileIndices() const
    {
        return m_vTileIndices;
    }
private:
    /* general attributes */
    Vei2 m_sizeInTiles = { 1, 1 };      /* number of tiles in x- and y-direction the building occupies */
    Type m_type;
    int m_life;
    const Level& m_level;

    /* graphics */
    const Surface& m_image;
    
    std::vector< int > m_vTileIndices;  /* indices of the building tiles in world (level) */
};