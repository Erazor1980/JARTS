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
    m_vEntities.push_back( Entity( { 2, 2 }, &m_level ) );
    /*m_vEntities.push_back( Entity( { 54, 54 } ) );
    m_vEntities.push_back( Entity( { 76, 318 } ) );*/
    
#if PATH_FINDING_TEST
    testPath = m_pathFinder.getShortestPath( test_start_idx, test_target_idx );
#endif
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

        /* test path */
#if PATH_FINDING_TEST
        if( e.GetType() == Mouse::Event::Type::LPress )
        {
            test_target_idx = m_level.getTileIdx( e.GetPosX(), e.GetPosY() );
            testPath = testFinder.getShortestPath( test_start_idx, test_target_idx );
        }
        else if( e.GetType() == Mouse::Event::Type::RPress )
        {
            test_start_idx = m_level.getTileIdx( e.GetPosX(), e.GetPosY() );
            testPath = testFinder.getShortestPath( test_start_idx, test_target_idx );
        }
#endif
    }
}

void Game::ComposeFrame()
{
    m_level.drawTileGrid( gfx, { 0, 0 } );

    for( const auto& e : m_vEntities )
    {
        e.draw( gfx );
    }

#if PATH_FINDING_TEST
    m_level.drawPath( gfx, testPath, test_start_idx, test_target_idx );
#endif

	//link.Draw( gfx );
}
