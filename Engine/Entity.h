#pragma once
#include "Graphics.h"

class Entity
{
public:
    Entity();

    void draw( Graphics& gfx ) const;
private:
    Vei2 m_pos;     // position in level in pixel coordinates
};