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
#include "Animation.h"
#include "FrameTimer.h"
#include "Character.h"
#include "Level.h"
#include "Unit.h"
#include "PathFinding.h"
#include "Font.h"
#include "Defines.h"
#include "Cursor.h"

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
	/********************************/
private:
	MainWindow& wnd;
	Graphics gfx;
	/********************************/
	/*  User Variables              */
	FrameTimer ft;
    Font m_font;
    Level m_level;
    PathFinder m_pathFinder;

    RectI m_selection;
    bool m_bSelecting = false;

    std::vector< Surface > m_vUnitSprites;
    std::vector< Sound > m_vSelectionSounds;
    std::vector< Sound > m_vCommandSounds;
    std::vector< Unit > m_vUnits;
    std::vector< Unit* > m_vpEnemies;

    /* Cursor stuff */
    Cursor m_cursor;

    bool m_bDrawDebugStuff = false;
	/********************************/
};