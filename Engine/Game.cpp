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
    m_pathFinder( m_level )
{
    m_vEntities.push_back( Entity( { 2, 2 }, &m_level, &m_pathFinder ) );
    m_vEntities.push_back( Entity( { 6, 5 }, &m_level, &m_pathFinder ) );
    m_vEntities.push_back( Entity( { 7, 8 }, &m_level, &m_pathFinder ) );    
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
    while( !wnd.mouse.IsEmpty() )
    {
        const Mouse::Event e = wnd.mouse.Read();
        for( auto &ent : m_vEntities )
        {
            ent.update( e.GetType(), Vec2( ( float )wnd.mouse.GetPosX(), ( float )wnd.mouse.GetPosY() ), wnd.kbd.KeyIsPressed( VK_SHIFT ), dt );
        }
    }
}

void Game::ComposeFrame()
{
    m_level.drawTileGrid( gfx, { 0, 0 } );

    for( const auto& e : m_vEntities )
    {
        e.draw( gfx );
    }
}
