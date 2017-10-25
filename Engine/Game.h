/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.h																				  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "Sound.h"
#include "Surface.h"
#include "FrameTimer.h"
#include "Level.h"
#include "Unit.h"
#include "PathFinding.h"
#include "Font.h"
#include "Defines.h"
#include "Cursor.h"
#include "SurfaceSequence.h"
#include "ActionBar.h"

class Game
{
public:
	Game( class MainWindow& wnd );
    ~Game();
	Game( const Game& ) = delete;
	Game& operator=( const Game& ) = delete;
	void Go();
private:
	void ComposeFrame();
	void UpdateModel();
	/********************************/
	/*  User Functions              */
    void drawAllUnits();
    void checkForDestroyedUnits();
    void handleMouse();
    void clearMemory();
    void restartGame();
    void updateCamera( const float dt );
    void updateKeyboard( const float dt );
    bool unitSelected();    /* checks if at least one unit is selected (no right-button-mouse-scrolling then) */
	/********************************/
private:
	MainWindow& wnd;
	Graphics gfx;
	/********************************/
	/*  User Variables              */
    ActionBar m_actionBar;
    FrameTimer ft;
    Font m_font;
    Level m_level;
    PathFinder m_pathFinder;

    RectI m_selection;
    bool m_bSelecting = false;

    std::vector< Surface > m_vTankSprites;
    std::vector< Surface > m_vJetSprites;
    std::vector< Surface > m_vSoldierSprites;
    std::vector< Sound > m_vTankSounds;
    std::vector< Sound > m_vJetSounds;
    std::vector< Sound > m_vSoldierSounds;
    std::vector< Unit* > m_vpUnits;
    
    Sound m_backGroundSound;

    Surface m_explSeqSprite;
    std::vector< SurfaceSequence* > m_vpDeathSequences;

    bool m_bDrawLifeBars = true;
    bool m_bDrawDebugStuff = false;
    
    /* Scrolling */
    Vei2 m_camPos = Vei2( Graphics::halfScreenWidth, Graphics::halfScreenHeight );
    /* when mouse is outside this rectangle we move the camera */
    const float m_scrolling_border = 50;
    RectF m_scrolling_rect = RectF( m_scrolling_border, Graphics::ScreenWidth - m_scrolling_border, m_scrolling_border, Graphics::ScreenHeight - m_scrolling_border );
    bool m_bScrollingPressed = false;
    Vec2 m_scrollingStartPos;
    bool m_bStartedScrolling = false;
    
    /* Cursor stuff */
    Cursor m_cursor;
	/********************************/
};