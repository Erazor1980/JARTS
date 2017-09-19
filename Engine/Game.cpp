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
    m_tankSprites( "..\\images\\tank_40x40_blue.bmp" ),
    m_pathFinder( m_level )
{
    //Surface tanks( "..\\images\\tank_40x40.bmp" );
    const int size = m_tankSprites.GetHeight();
    for( int i = 0; i < 8; ++i )
    {
        m_vSpriteRects.emplace_back( i * size, ( i + 1 ) * size, 0, size );
    }
    m_vEntities.push_back( Entity( { 3, 3 }, &m_level, &m_pathFinder, m_tankSprites, m_vSpriteRects ) );
    m_vEntities.push_back( Entity( { 17, 2 }, &m_level, &m_pathFinder, m_tankSprites, m_vSpriteRects ) );
    m_vEntities.push_back( Entity( { 13, 13 }, &m_level, &m_pathFinder, m_tankSprites, m_vSpriteRects ) );
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
            for( auto &ent : m_vEntities )
            {
                ent.update( e.GetType(), Vec2( ( float )wnd.mouse.GetPosX(), ( float )wnd.mouse.GetPosY() ), wnd.kbd.KeyIsPressed( VK_SHIFT ), dt );
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
            }
            if( e.GetType() == Mouse::Event::Type::LRelease )
            {
                m_bSelecting = false;
                for( auto &ent : m_vEntities )
                {
                    if( m_selection.Contains( ent.getPosition() ) )
                    {
                        ent.select();
                    }
                }
            }
        }
    }
    else
    {
        const Mouse::Event e = wnd.mouse.Read();
        for( auto &ent : m_vEntities )
        {
            ent.update( e.GetType(), Vec2( ( float )wnd.mouse.GetPosX(), ( float )wnd.mouse.GetPosY() ), wnd.kbd.KeyIsPressed( VK_SHIFT ), dt );
        }
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
}

void Game::ComposeFrame()
{
    m_level.draw( gfx, m_bDrawDebugStuff );

    for( const auto& e : m_vEntities )
    {
        e.draw( gfx, m_bDrawDebugStuff );
    }

    if( m_bSelecting )
    {
        gfx.DrawRectBorder( m_selection.getNormalized(), 1, Colors::White );
    }
}
