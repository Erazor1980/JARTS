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
    font( "..\\images\\Fixedsys16x28.bmp" )
{
    vVehicles.push_back( Vehicle( 120.0f, 120.0f ) );
    std::vector< Vec2 > vPoints = { { 50, 100 }, { 250, 140 }, { 390, 80 }, { 550, 230 }, { 600, 400 }, { 300, 300 }, { 50, 200 } };
    testPath = Path( vPoints, 10 );
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

    //testVehicle.follow( { 100, 100 }, { 100, 500 }, 10, dt );
    for( auto& v : vVehicles )
    {
        v.separate( vVehicles, dt );
        if( bPathMode )
        {
            v.followPath( testPath, dt );
        }
        else
        {
            v.moveToTarget( wnd.mouse.GetPos(), dt );
        }
    }

    while( !wnd.mouse.IsEmpty() )
    {
        const Mouse::Event e = wnd.mouse.Read();
        if( e.GetType() == Mouse::Event::Type::LPress )
        {
            vVehicles.push_back( Vehicle( ( float )wnd.mouse.GetPosX(), ( float )wnd.mouse.GetPosY() ) );
        }
    }

    while( !wnd.kbd.KeyIsEmpty() )
    {
        const Keyboard::Event e = wnd.kbd.ReadKey();
        if( e.IsRelease() )
        {
            if( e.GetCode() == VK_SPACE )
            {
                bPathMode = !bPathMode;
            }
        }
    }
}

void Game::ComposeFrame()
{
    testPath.draw( gfx );

    for( auto& v : vVehicles )
    {
        v.draw( gfx );
    }

    char text[ 100 ];
    sprintf_s( text, "path idx: %d", vVehicles.front().m_pathIdx );
    font.DrawText( text, { 20, gfx.ScreenHeight - 35 }, Colors::Green, gfx );

    sprintf_s( text, "speed: %0.2f", vVehicles.front().m_velocity.GetLength() );
    font.DrawText( text, { 230, gfx.ScreenHeight - 35 }, Colors::Green, gfx );

    sprintf_s( text, "#: %d", ( int )vVehicles.size() );
    font.DrawText( text, { 430, gfx.ScreenHeight - 35 }, Colors::Green, gfx );

    if( bPathMode )
    {
        sprintf_s( text, "Path mode" );
    }
    else
    {
        sprintf_s( text, "Mouse mode" );
    }
    font.DrawText( text, { gfx.ScreenWidth / 2 - 60, 15 }, Colors::Red, gfx );
}
