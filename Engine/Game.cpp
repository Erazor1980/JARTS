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
    m_font( "..\\images\\Fixedsys16x28.bmp" ),
#if _DEBUG
    m_level( "..\\images\\maps\\testLvl1_debug.bmp" ),
#else
    m_level( "..\\images\\maps\\testLvl1_800x600.bmp" ),
#endif
    m_pathFinder( m_level ),
    m_cursor( gfx, wnd.mouse, m_vUnits, m_vEnemies, m_level )
{
    srand( ( unsigned int )time( NULL ) );

    /* load images */
    m_vUnitSprites.push_back( Surface( "..\\images\\units\\tank_40x40_blue.bmp" ) );
    m_vUnitSprites.push_back( Surface( "..\\images\\units\\tank_40x40_blue.bmp" ) );   //TODO replace by soldier sprite later
    m_vUnitSprites.push_back( Surface( "..\\images\\units\\jet_40x40.bmp" ) );
    m_vUnitSprites.push_back( Surface( "..\\images\\units\\tank_40x40_red.bmp" ) );
    
    /* load sounds */
    m_vSelectionSounds.push_back( Sound( L"..\\sounds\\ready_for_duty.wav" ) );
    m_vSelectionSounds.push_back( Sound( L"..\\sounds\\yes_sir.wav" ) );

    m_vCommandSounds.push_back( Sound( L"..\\sounds\\move_tank.wav" ) );
    m_vCommandSounds.push_back( Sound( L"..\\sounds\\move_jet.wav" ) );

    /* create units */
    m_vUnits.push_back( Unit( { 2, 7 }, &m_level, &m_pathFinder, UnitType::TANK, m_vUnitSprites[ 0 ], m_vSelectionSounds[ 0 ], m_vCommandSounds[ 0 ] ) );
    m_vUnits.push_back( Unit( { 5, 7 }, &m_level, &m_pathFinder, UnitType::TANK, m_vUnitSprites[ 0 ], m_vSelectionSounds[ 0 ], m_vCommandSounds[ 0 ] ) );
    m_vUnits.push_back( Unit( { 2, 8 }, &m_level, &m_pathFinder, UnitType::TANK, m_vUnitSprites[ 0 ], m_vSelectionSounds[ 0 ], m_vCommandSounds[ 0 ] ) );
    //m_vUnits.push_back( Unit( { 5, 6 }, &m_level, &m_pathFinder, UnitType::TANK, m_vUnitSprites, m_vSelectionSounds[ 0 ], m_vCommandSounds[ 0 ] ) );
    m_vUnits.push_back( Unit( { 13, 13 }, &m_level, &m_pathFinder, UnitType::JET, m_vUnitSprites[ 2 ], m_vSelectionSounds[ 1 ], m_vCommandSounds[ 1 ] ) );
    m_vUnits.push_back( Unit( { 10, 13 }, &m_level, &m_pathFinder, UnitType::JET, m_vUnitSprites[ 2 ], m_vSelectionSounds[ 1 ], m_vCommandSounds[ 1 ] ) );

    /* create enemies */
    m_vEnemies.push_back( Unit( { 10, 8 }, &m_level, &m_pathFinder, UnitType::TANK, m_vUnitSprites[ 3 ], m_vSelectionSounds[ 0 ], m_vCommandSounds[ 0 ] ) );

    /* disable windows standard cursor (we want to use our own) */
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
                u.update( m_vUnits, e.GetType(), wnd.mouse.GetPos(), wnd.kbd.KeyIsPressed( VK_SHIFT ), dt );
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
                    if( m_selection.Contains( u.getLocation() ) )
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
            u.update( m_vUnits, dt );
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
    m_cursor.update( dt );
}

void Game::drawAllUnits()
{
    /* draw all ground units first */
    for( int i = 0; i < m_vUnits.size(); ++i )
    {
        if( m_vUnits[ i ].isGroundUnit() )
        {
            m_vUnits[ i ].draw( gfx, m_bDrawDebugStuff );
        }

#if _DEBUG
        m_font.DrawText( std::to_string( m_vUnits[ i ].getTileIdx() ), m_vUnits[ i ].getLocationInt() - Vei2( 10, 10 ), Colors::White, gfx );
        m_font.DrawText( std::to_string( i ), m_vUnits[ i ].getLocationInt() - Vei2( 30, 10 ), Colors::Red, gfx );
#endif
    }
    for( int i = 0; i < m_vEnemies.size(); ++i )
    {
        if( m_vEnemies[ i ].isGroundUnit() )
        {
            m_vEnemies[ i ].draw( gfx );
        }
    }
    

    /* draw all air units */
    for( int i = 0; i < m_vUnits.size(); ++i )
    {
        if( !m_vUnits[ i ].isGroundUnit() )
        {
            m_vUnits[ i ].draw( gfx, m_bDrawDebugStuff );
        }

#if _DEBUG
        m_font.DrawText( std::to_string( m_vUnits[ i ].getTileIdx() ), m_vUnits[ i ].getLocationInt() - Vei2( 10, 10 ), Colors::White, gfx );
        m_font.DrawText( std::to_string( i ), m_vUnits[ i ].getLocationInt() - Vei2( 30, 10 ), Colors::Red, gfx );
#endif
    }
    for( int i = 0; i < m_vEnemies.size(); ++i )
    {
        if( !m_vEnemies[ i ].isGroundUnit() )
        {
            m_vEnemies[ i ].draw( gfx );
        }
    }
}

void Game::ComposeFrame()
{
    m_level.draw( gfx, m_bDrawDebugStuff );
    
    drawAllUnits();

    if( m_bSelecting )
    {
        gfx.DrawRectBorder( m_selection.getNormalized(), 1, Colors::White );
    }

#if _DEBUG  /* display additional unit infos */
    int x = 100;
    char text[ 50 ];
    for( int i = 0; i < m_vUnits.size(); ++i )
    {
        m_font.DrawText( std::to_string( i ), { x, 1 }, Colors::Cyan, gfx );

        sprintf_s( text, "%0.3f", m_vUnits[ i ].getVelocity().GetLength() );
        m_font.DrawText( text, { x, 30 }, Colors::Cyan, gfx );
        if( m_vUnits[ i ].getState() == Unit::State::MOVING )
        {
            m_font.DrawText( "Moving", { x, 60 }, Colors::Cyan, gfx );
        }
        else if( m_vUnits[ i ].getState() == Unit::State::STANDING )
        {
            m_font.DrawText( "Standing", { x, 60 }, Colors::Cyan, gfx );
        }
        else if( m_vUnits[ i ].getState() == Unit::State::WAITING )
        {
            m_font.DrawText( "Waiting", { x, 60 }, Colors::Cyan, gfx );
        }
        sprintf_s( text, "%0.3f", m_vUnits[ i ].getWaitingTime() );
        m_font.DrawText( text, { x, 90 }, Colors::Cyan, gfx );

        x += 150;
    }
#endif
    m_cursor.draw();
}
