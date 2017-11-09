#pragma once
#include <vector>
#include "Graphics.h"
#include "Surface.h"
#include "Mouse.h"
#include "Building.h"
#include "RectI.h"
#include "Font.h"
#include "Level.h"
#include "Unit.h"

class ActionBar
{
public:
    ActionBar( const std::vector< Surface >& vBuildingImages, const std::vector< Unit* >& vpUnits, const std::vector< Building >& vBuildings );

    void draw( Graphics& gfx, Font& font, const Vec2& mousePos );

    void update( const float dt, const Vec2& mousePos, const Vei2& camPos, const Level& level );

    void handleMouse( const Mouse::Event::Type& type, const Vec2& mousePos, std::vector< Building >& vBuildings, const Level& level );
    
    void cancelPlacing()
    {
        m_bPlacing = false;
    }

    int getWidth() const
    {
        return m_width;
    }
    bool isPlacing() const
    {
        return m_bPlacing;
    }
private:
    /* images and width of the action bar */
    Surface m_img;
    const std::vector< Surface >& m_vBuildingImages;
    int m_width;

    /* current building attributes */
    Building::Type m_buildingType;
    Vei2 m_buildingSize;
    bool m_bPlacing = false;
    bool m_bFreeSpace = false;

    /* placing attributes */
    std::vector< RectF > m_vBuildingTiles;  /* rects in screen coordinates */
    std::vector< bool > m_vTileOccupancy;   /* true = free, false = occupied */
    std::vector< int > m_vBuildingIndices;  /* indices of current building in map (for placing) */

    /* rectangles for specific buildings/units inside the action bar */
    std::vector< RectI > m_vRectsInBar_buildings;
    std::vector< RectI > m_vRectsInBar_units;

    /* all units and buildings in the current map (for free space test) */
    const std::vector< Unit* >& m_vpUnits;
    const std::vector< Building >& m_vBuildings;
};

