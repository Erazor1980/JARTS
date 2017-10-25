#pragma once

#include "Graphics.h"
#include "Surface.h"

class ActionBar
{
public:
    ActionBar();

    void draw( Graphics& gfx ) const;

    int getWidth() const
    {
        return m_width;
    }
private:
    Surface m_img;
    int m_width;
};

