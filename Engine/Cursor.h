#pragma once
#include "Graphics.h"
#include "Level.h"
#include "Unit.h"
#include "Mouse.h"

class Cursor
{
public:
    Cursor( Graphics& gfx, const Mouse& mouse, const std::vector< Unit* >& vpUnits, const Level& level );

    void update( const float dt );
    void draw();
private:
    const Surface m_mainSprite;
    const Surface m_forbiddenSprite;

    bool m_bUnitSelected        = false;
    bool m_bSelectedGroundUnit  = false;
    bool m_bMouseOverUnit       = false;
    bool m_bMouseOverEnemy      = false;
    bool m_bCursorBlinkShow     = false;
    float m_cursorBlinkDelta    = 0.3f;     // time between blink off/on of cursor in seconds
    float m_cursorBlinkTime     = 0;        // current on/off time
    RectF m_rectFromUnit;                   // for mouse over unit blinking

    /* movement / attack animations */
    void advanceAnimation();
    const int m_maxAnimations   = 5;
    float m_animationTime       = 0;
    float m_animationDelta      = 0.1f;
    int m_animationIdx          = 0;
    RectF m_animationRect;

    Graphics& m_gfx;
    const Mouse& m_mouse;
    const std::vector< Unit* >& m_vpUnits;
    //const std::vector< Unit* >& m_vpEnemies;
    const Level& m_level;
};