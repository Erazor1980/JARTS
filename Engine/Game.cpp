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

#include "PathFinding.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd )
{
    m_vEntities.push_back( Entity( { 142, 120 } ) );
    m_vEntities.push_back( Entity( { 54, 54 } ) );
    m_vEntities.push_back( Entity( { 76, 318 } ) );
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
    /////////////////
    ///// MOUSE /////
    /////////////////
    while( !wnd.mouse.IsEmpty() )
    {
        const Mouse::Event e = wnd.mouse.Read();
        for( auto &ent : m_vEntities )
        {
            ent.update( e.GetType(), Vec2( ( float )wnd.mouse.GetPosX(), ( float )wnd.mouse.GetPosY() ), wnd.kbd.KeyIsPressed( VK_SHIFT ) );
        }
    }

    PathFinder test( testLvl );
    test.getShortestPath( 0, 5 );

	//Vec2 dir = { 0.0f,0.0f };
	//if( wnd.kbd.KeyIsPressed( VK_UP ) )
	//{
	//	dir.y -= 1.0f;
	//}
	//if( wnd.kbd.KeyIsPressed( VK_DOWN ) )
	//{
	//	dir.y += 1.0f;
	//}
	//if( wnd.kbd.KeyIsPressed( VK_LEFT ) )
	//{
	//	dir.x -= 1.0f;
	//}
	//if( wnd.kbd.KeyIsPressed( VK_RIGHT ) )
	//{
	//	dir.x += 1.0f;
	//}
	//link.SetDirection( dir );
	//link.Update( ft.Mark() );
}

void Game::ComposeFrame()
{
    testLvl.drawTileGrid( gfx, { 0, 0 } );

    for( const auto& e : m_vEntities )
    {
        e.draw( gfx );
    }

	//link.Draw( gfx );
}
