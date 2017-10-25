#pragma once
#include <vector>
#include "Graphics.h"
#include "Surface.h"
#include "Mouse.h"
#include "Building.h"
#include "RectI.h"
#include "Font.h"
#include "Level.h"

class ActionBar
{
public:
    ActionBar();

    void draw( Graphics& gfx, Font& font, const Vec2& mousePos );

    void update( const float dt, const Vec2& mousePos, const Vei2& camPos, const Level& level );

    void handleMouse( const Mouse::Event::Type& type, const Vec2& mousePos );
    
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
    /* image and width of the action bar */
    Surface m_img;
    int m_width;

    /* current building attributes */
    Building::Type m_buildingType;
    Vei2 m_buildingSize;
    bool m_bPlacing = false;
    bool m_bFreeSpace = false;

    /* placing tiles and colors */
    std::vector< RectF > m_vBuildingTiles;
    std::vector< bool > m_vTileOccupancy;   /* true = free, false = occupied */

    /* rectangles for specific buildings inside the action bar */
    std::vector< RectI > m_vBuildingRects;
};

