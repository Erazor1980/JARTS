/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
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
#include "MainWindow.h"
#include "Game.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
#if _DEBUG
    m_level( "..\\images\\testLvl1_debug.bmp" ),
#else
    m_level( "..\\images\\testLvl1_800x600.bmp" ),
#endif
    m_cursorSprite( "..\\images\\cursor.bmp" ),
    m_pathFinder( m_level ),
    m_pathControlling( m_level )
{
    /* load images */
    m_vUnitSprites.push_back( Surface( "..\\images\\tank_40x40_blue.bmp" ) );
    m_vUnitSprites.push_back( Surface( "..\\images\\tank_40x40_blue.bmp" ) );   /* will be replaced by soldier sprite later */
    m_vUnitSprites.push_back( Surface( "..\\images\\jet_40x40.bmp" ) );
    
    /* load sounds */
    m_vSelectionSounds.push_back( Sound( L"..\\sounds\\ready_for_duty.wav" ) );
    m_vSelectionSounds.push_back( Sound( L"..\\sounds\\yes_sir.wav" ) );

    m_vCommandSounds.push_back( Sound( L"..\\sounds\\move_tank.wav" ) );
    m_vCommandSounds.push_back( Sound( L"..\\sounds\\move_jet.wav" ) );

    /* create units */
    m_vUnits.push_back( Unit( { 1, 13 }, &m_level, &m_pathFinder, UnitType::TANK, m_vUnitSprites, m_vSelectionSounds[ 0 ], m_vCommandSounds[ 0 ] ) );
    m_vUnits.push_back( Unit( { 1, 14 }, &m_level, &m_pathFinder, UnitType::TANK, m_vUnitSprites, m_vSelectionSounds[ 0 ], m_vCommandSounds[ 0 ] ) );
    m_vUnits.push_back( Unit( { 3, 13 }, &m_level, &m_pathFinder, UnitType::TANK, m_vUnitSprites, m_vSelectionSounds[ 0 ], m_vCommandSounds[ 0 ] ) );
    m_vUnits.push_back( Unit( { 11, 11 }, &m_level, &m_pathFinder, UnitType::JET, m_vUnitSprites, m_vSelectionSounds[ 1 ], m_vCommandSounds[ 1 ] ) );
    m_vUnits.push_back( Unit( { 14, 14 }, &m_level, &m_pathFinder, UnitType::JET, m_vUnitSprites, m_vSelectionSounds[ 1 ], m_vCommandSounds[ 1 ] ) );

    ShowCursor( false );
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
    const float dt = ft.Mark();
    
    m_pathControlling.update( m_vUnits );

    /////////////////
    ///// MOUSE /////
    /////////////////
    if( wnd.mouse.IsInWindow() && !wnd.mouse.IsEmpty() )
    {
        while( !wnd.mouse.IsEmpty() )
        {
            const Mouse::Event e = wnd.mouse.Read();
            for( auto &u : m_vUnits )
            {
                u.update( e.GetType(), wnd.mouse.GetPos(), wnd.kbd.KeyIsPressed( VK_SHIFT ), dt );
            }

            /* multi selection rectangle */
            if( e.GetType() == Mouse::Event::Type::LPress )
            {
                m_selection.left = wnd.mouse.GetPosX();
                m_selection.top = wnd.mouse.GetPosY();
            }
            if( e.LeftIsPressed() )
            {
                m_bSelecting = true;
                m_selection.right = wnd.mouse.GetPosX();
                m_selection.bottom = wnd.mouse.GetPosY();

                for( auto &u : m_vUnits )
                {
                    u.handleSelectionRect( m_selection );
                }
            }
            if( e.GetType() == Mouse::Event::Type::LRelease && m_bSelecting )
            {
                m_bSelecting = false;
                for( auto &u : m_vUnits )
                {
                    if( m_selection.Contains( u.getPosition() ) )
                    {
                        u.select();
                    }
                }
            }
        }
    }
    else
    {
        for( auto &u : m_vUnits )
        {
            u.update( dt );
        }
    }

    if( !wnd.mouse.IsInWindow() )
    {
        m_bSelecting = false;
    }

    //////////////////
    //// KEYBOARD ////
    //////////////////
    while( !wnd.kbd.KeyIsEmpty() )
    {
        const Keyboard::Event e = wnd.kbd.ReadKey();
        if( e.IsRelease() )
        {
            if( e.GetCode() == VK_SPACE )
            {
                m_bDrawDebugStuff = !m_bDrawDebugStuff;
            }
        }
    }

    /* Cursor */
    if( m_bMouseOverUnit )
    {
        m_cursorBlinkTime += dt;
        if( m_cursorBlinkTime >= m_cursorBlinkDelta )
        {
            m_bCursorBlinkShow = !m_bCursorBlinkShow;
            m_cursorBlinkTime = 0;
        }
    }
}

void Game::drawMouseCurser()
{
    Vec2 mp = wnd.mouse.GetPos();
    m_bMouseOverUnit = false;
    for( const auto &u : m_vUnits )
    {
        RectF bb = u.getBoundigBox();
        if( bb.Contains( mp ) )
        {
            if( m_bCursorBlinkShow )
            {
                gfx.DrawRectCorners( bb, Colors::White );
            }
            m_bMouseOverUnit = true;
            break;
        }
    }
    if( wnd.mouse.IsInWindow() && !m_bMouseOverUnit )
    {
        gfx.DrawSprite( ( int )mp.x, ( int )mp.y, m_cursorSprite, { 255, 242, 0 } );
        m_cursorBlinkTime = 0;
        m_bCursorBlinkShow = true;
    }
}

void Game::ComposeFrame()
{
    m_level.draw( gfx, m_bDrawDebugStuff );

    for( const auto& u : m_vUnits )
    {
        u.draw( gfx, m_bDrawDebugStuff );
    }

    if( m_bSelecting )
    {
        gfx.DrawRectBorder( m_selection.getNormalized(), 1, Colors::White );
    }

    drawMouseCurser();
}
