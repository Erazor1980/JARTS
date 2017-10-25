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

    void draw( Graphics& gfx, Font& font, const Vec2& mouse_pos, const Level& level, const Vei2& camPos ) const;

    void handleMouse( const Mouse::Event::Type& type, const Vec2& mouse_pos );
    
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

    /* current building type to build/place */
    Building::Type m_buildingType;
    bool m_bPlacing = false;

    /* rectangles for specific buildings inside the action bar */
    std::vector< RectI > m_vBuildingRects;
};

