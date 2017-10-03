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
    m_cursor( gfx, wnd.mouse, m_vpUnits, m_level )
{
    srand( ( unsigned int )time( NULL ) );

    /* load images */
    m_vUnitSprites.push_back( Surface( "..\\images\\units\\tank_40x40_blue.bmp" ) );
    m_vUnitSprites.push_back( Surface( "..\\images\\units\\tank_40x40_blue.bmp" ) );   //TODO replace by soldier sprite later
    m_vUnitSprites.push_back( Surface( "..\\images\\units\\jet_40x40.bmp" ) );
    m_vUnitSprites.push_back( Surface( "..\\images\\units\\tank_40x40_red.bmp" ) );
    
    /* load sounds - order important! selection -> command -> attack -> death */
    m_vTankSounds.push_back( Sound( L"..\\sounds\\ready_for_duty.wav" ) );
    m_vTankSounds.push_back( Sound( L"..\\sounds\\move_tank.wav" ) );
    m_vTankSounds.push_back( Sound( L"..\\sounds\\tank_firing.wav" ) );
    m_vTankSounds.push_back( Sound( L"..\\sounds\\explosion1.wav" ) );
    
    m_vJetSounds.push_back( Sound( L"..\\sounds\\yes_sir.wav" ) );
    m_vJetSounds.push_back( Sound( L"..\\sounds\\move_jet.wav" ) );
    m_vJetSounds.push_back( Sound( L"..\\sounds\\jet_firing.wav" ) );
    m_vJetSounds.push_back( Sound( L"..\\sounds\\explosion1.wav" ) );

    /* create units */
    m_vpUnits.push_back( new Unit( { 2, 7 }, Team::_A, m_level, m_pathFinder, m_vpUnits, UnitType::TANK, m_vUnitSprites[ 0 ], m_vTankSounds ) );
    m_vpUnits.push_back( new Unit( { 5, 7 }, Team::_A, m_level, m_pathFinder, m_vpUnits, UnitType::TANK, m_vUnitSprites[ 0 ], m_vTankSounds ) );
    m_vpUnits.push_back( new Unit( { 2, 8 }, Team::_A, m_level, m_pathFinder, m_vpUnits, UnitType::TANK, m_vUnitSprites[ 0 ], m_vTankSounds ) );
    m_vpUnits.push_back( new Unit( { 13, 13 }, Team::_A, m_level, m_pathFinder, m_vpUnits, UnitType::JET, m_vUnitSprites[ 2 ], m_vJetSounds ) );
    m_vpUnits.push_back( new Unit( { 10, 13 }, Team::_A, m_level, m_pathFinder, m_vpUnits, UnitType::JET, m_vUnitSprites[ 2 ], m_vJetSounds ) );

    /* create enemies */
    m_vpUnits.push_back( new Unit( { 10, 8 }, Team::_B, m_level, m_pathFinder, m_vpUnits, UnitType::TANK, m_vUnitSprites[ 3 ], m_vTankSounds ) );

    /* disable windows standard cursor (we want to use our own) */
    ShowCursor( false );
}

Game::~Game()
{
    for( int i = 0; i < m_vpUnits.size(); ++i )
    {
        delete m_vpUnits[ i ];
    }
    m_vpUnits.clear();
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
    checkForDestroyedUnits();

    const float dt = ft.Mark();
    /////////////////
    ///// MOUSE /////
    /////////////////
    handleMouse();
    
    ///////////////
    //// UNITS ////
    ///////////////
    for( auto &u : m_vpUnits )
    {
        u->update( dt );
    }

    /*for( auto &u : m_vpEnemies )
    {
        u->update( dt );
    }*/
    
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

    ////////////////
    //// CURSOR ////
    ////////////////
    m_cursor.update( dt );
}

void Game::drawAllUnits()
{
    /* draw all ground units first */
    for( int i = 0; i < m_vpUnits.size(); ++i )
    {
        if( m_vpUnits[ i ]->isGroundUnit() )
        {
            m_vpUnits[ i ]->draw( gfx, m_bDrawDebugStuff );
#if _DEBUG
            //m_font.DrawText( std::to_string( m_vUnits[ i ].getTileIdx() ), m_vUnits[ i ].getLocationInt() - Vei2( 10, 10 ), Colors::White, gfx );
            m_font.DrawText( std::to_string( i ), m_vpUnits[ i ]->getLocationInt() - Vei2( 30, 10 ), Colors::Red, gfx );
#endif
        }
    }

    /* draw all air units */
    for( int i = 0; i < m_vpUnits.size(); ++i )
    {
        if( !m_vpUnits[ i ]->isGroundUnit() )
        {
            m_vpUnits[ i ]->draw( gfx, m_bDrawDebugStuff );
#if _DEBUG
            //m_font.DrawText( std::to_string( m_vUnits[ i ].getTileIdx() ), m_vUnits[ i ].getLocationInt() - Vei2( 10, 10 ), Colors::White, gfx );
            m_font.DrawText( std::to_string( i ), m_vpUnits[ i ]->getLocationInt() - Vei2( 30, 10 ), Colors::Red, gfx );
#endif
        }
    }
}

void Game::checkForDestroyedUnits()
{
    auto u = m_vpUnits.begin();
    while( u != m_vpUnits.end() )
    {
        if( ( *u )->isDestroyed() )
        {
            u = m_vpUnits.erase( u );
        }
        else
        {
            u++;
        }
    }
}

void Game::handleMouse()
{
    if( wnd.mouse.IsInWindow() && !wnd.mouse.IsEmpty() )
    {
        while( !wnd.mouse.IsEmpty() )
        {
            const Mouse::Event e = wnd.mouse.Read();
            for( auto &u : m_vpUnits )
            {
                u->handleMouse( e.GetType(), wnd.mouse.GetPos(), wnd.kbd.KeyIsPressed( VK_SHIFT ) );
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

                for( auto &u : m_vpUnits )
                {
                    u->handleSelectionRect( m_selection );
                }
            }
            if( e.GetType() == Mouse::Event::Type::LRelease && m_bSelecting )
            {
                m_bSelecting = false;
                for( auto &u : m_vpUnits )
                {
                    if( u->getTeam() != Team::_A )
                    {
                        continue;
                    }
                    if( m_selection.Contains( u->getLocation() ) )
                    {
                        u->select();
                    }
                }
            }
        }
    }

    if( !wnd.mouse.IsInWindow() )
    {
        m_bSelecting = false;
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
    for( int i = 0; i < m_vpUnits.size(); ++i )
    {
        m_font.DrawText( std::to_string( i ), { x, 1 }, Colors::Cyan, gfx );

        sprintf_s( text, "%0.3f", m_vpUnits[ i ]->getVelocity().GetLength() );
        m_font.DrawText( text, { x, 30 }, Colors::Cyan, gfx );
        if( m_vpUnits[ i ]->getState() == Unit::State::MOVING )
        {
            m_font.DrawText( "Moving", { x, 60 }, Colors::Cyan, gfx );
        }
        else if( m_vpUnits[ i ]->getState() == Unit::State::STANDING )
        {
            m_font.DrawText( "Standing", { x, 60 }, Colors::Cyan, gfx );
        }
        else if( m_vpUnits[ i ]->getState() == Unit::State::WAITING )
        {
            m_font.DrawText( "Waiting", { x, 60 }, Colors::Cyan, gfx );
        }
        else if( m_vpUnits[ i ]->getState() == Unit::State::ATTACKING )
        {
            m_font.DrawText( "Attacking", { x, 60 }, Colors::Cyan, gfx );
        }
        sprintf_s( text, "%0.3f", m_vpUnits[ i ]->getWaitingTime() );
        m_font.DrawText( text, { x, 90 }, Colors::Cyan, gfx );

        x += 150;
    }
#endif
    m_cursor.draw();
}
