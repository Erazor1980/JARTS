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
    m_level( "..\\images\\debugImg.bmp", m_actionBar.getWidth() ),
#else
    m_level( "..\\images\\maps\\desert.bmp", m_actionBar.getWidth() ),
#endif
    m_pathFinder( m_level ),
    m_cursor( gfx, wnd.mouse, m_vpUnits, m_level, m_scrolling_rect, m_actionBar.getWidth() ),
    m_explSeqSprite( "..\\images\\effects\\expl_seq.bmp" )
{
    srand( ( unsigned int )time( NULL ) );

    /* load images */
    m_vTankSprites = { Surface( "..\\images\\units\\tank_40x40.bmp" ), Surface( "..\\images\\effects\\expl_1.bmp" ), Surface( "..\\images\\effects\\expl_seq.bmp" ) };
    m_vJetSprites = { Surface( "..\\images\\units\\jet_40x40.bmp" ), Surface( "..\\images\\effects\\expl_1.bmp" ), Surface( "..\\images\\effects\\expl_seq.bmp" ) };
    
    /* load sounds - order important! selection -> command -> attack -> death */
    m_vTankSounds.push_back( Sound( L"..\\sounds\\ready_for_duty.wav" ) );
    m_vTankSounds.push_back( Sound( L"..\\sounds\\move_tank.wav" ) );
    m_vTankSounds.push_back( Sound( L"..\\sounds\\tank_firing.wav" ) );
    m_vTankSounds.push_back( Sound( L"..\\sounds\\explosion1.wav" ) );
    
    m_vJetSounds.push_back( Sound( L"..\\sounds\\yes_sir.wav" ) );
    m_vJetSounds.push_back( Sound( L"..\\sounds\\move_jet.wav" ) );
    m_vJetSounds.push_back( Sound( L"..\\sounds\\jet_firing.wav" ) );
    m_vJetSounds.push_back( Sound( L"..\\sounds\\explosion1.wav" ) );

    m_backGroundSound = Sound( L"..\\sounds\\background_wind.wav", 0.0f, 59.9f );

    /* disable windows standard cursor (we want to use our own) */
    ShowCursor( false );

    restartGame();
}

Game::~Game()
{
    clearMemory();
}
void Game::clearMemory()
{
    for( int i = 0; i < m_vpUnits.size(); ++i )
    {
        delete m_vpUnits[ i ];
    }
    m_vpUnits.clear();
}
void Game::restartGame()
{
#if !_DEBUG
    m_backGroundSound.Play( 1.0f, 0.5f );
#endif

    clearMemory();

    /* create units */
    m_vpUnits.push_back( new Unit( { 3, 5 }, Team::_A, m_level, m_pathFinder, m_vpUnits, UnitType::TANK, m_vTankSprites, m_vTankSounds ) );
    m_vpUnits.push_back( new Unit( { 2, 3 }, Team::_A, m_level, m_pathFinder, m_vpUnits, UnitType::TANK, m_vTankSprites, m_vTankSounds ) );
    m_vpUnits.push_back( new Unit( { 14, 3 }, Team::_A, m_level, m_pathFinder, m_vpUnits, UnitType::TANK, m_vTankSprites, m_vTankSounds ) );
    m_vpUnits.push_back( new Unit( { 32, 3 }, Team::_A, m_level, m_pathFinder, m_vpUnits, UnitType::TANK, m_vTankSprites, m_vTankSounds ) );
    m_vpUnits.push_back( new Unit( { 34, 7 }, Team::_A, m_level, m_pathFinder, m_vpUnits, UnitType::TANK, m_vTankSprites, m_vTankSounds ) );
    m_vpUnits.push_back( new Unit( { 7, 2 }, Team::_A, m_level, m_pathFinder, m_vpUnits, UnitType::JET, m_vJetSprites, m_vJetSounds ) );
    m_vpUnits.push_back( new Unit( { 10, 4 }, Team::_A, m_level, m_pathFinder, m_vpUnits, UnitType::JET, m_vJetSprites, m_vJetSounds ) );

    /* create enemies */
    m_vpUnits.push_back( new Unit( { 7, 12 }, Team::_B, m_level, m_pathFinder, m_vpUnits, UnitType::TANK, m_vTankSprites, m_vTankSounds ) );
    m_vpUnits.push_back( new Unit( { 17, 13 }, Team::_B, m_level, m_pathFinder, m_vpUnits, UnitType::JET, m_vJetSprites, m_vJetSounds ) );
    m_vpUnits.push_back( new Unit( { 13, 13 }, Team::_B, m_level, m_pathFinder, m_vpUnits, UnitType::JET, m_vJetSprites, m_vJetSounds ) );
    m_vpUnits.push_back( new Unit( { 27, 17 }, Team::_B, m_level, m_pathFinder, m_vpUnits, UnitType::TANK, m_vTankSprites, m_vTankSounds ) );
    m_vpUnits.push_back( new Unit( { 33, 15 }, Team::_B, m_level, m_pathFinder, m_vpUnits, UnitType::JET, m_vJetSprites, m_vJetSounds ) );
    m_vpUnits.push_back( new Unit( { 31, 13 }, Team::_B, m_level, m_pathFinder, m_vpUnits, UnitType::JET, m_vJetSprites, m_vJetSounds ) );

    /* reset camera position */
    m_camPos = Vei2( Graphics::halfScreenWidth, Graphics::halfScreenHeight );
}
void Game::updateCamera( const float dt )
{
    if( m_bSelecting )
    {
        return;     /* not allowing to scroll while selecting units with rectangle */
    }

    Vec2 mp = wnd.mouse.GetPos();
    
    if( m_bScrollingPressed )
    {
        Vec2 d = m_scrollingStartPos - mp;

        m_camPos.x += ( int )d.x;
        m_camPos.y += ( int )d.y;

        m_scrollingStartPos = mp;
    }
    else if( wnd.mouse.IsInWindow() )
    {
        const int pixelsToMove = int( 500.0f * dt );
        if( mp.x < m_scrolling_rect.left )
        {
            m_camPos.x -= pixelsToMove;
        }
        else if( mp.x > m_scrolling_rect.right )
        {
            m_camPos.x += pixelsToMove;
        }
        if( mp.y < m_scrolling_rect.top )
        {
            m_camPos.y -= pixelsToMove;
        }
        else if( mp.y > m_scrolling_rect.bottom )
        {
            m_camPos.y += pixelsToMove;
        }
    }

    m_camPos.x = std::max( Graphics::halfScreenWidth, m_camPos.x );
    m_camPos.x = std::min( m_level.getWidth() - Graphics::halfScreenWidth, m_camPos.x );
    m_camPos.y = std::max( Graphics::halfScreenHeight, m_camPos.y );
    m_camPos.y = std::min( m_level.getHeight() - Graphics::halfScreenHeight, m_camPos.y );
}
void Game::updateKeyboard( const float dt )
{
    while( !wnd.kbd.KeyIsEmpty() )
    {
        const Keyboard::Event e = wnd.kbd.ReadKey();
        if( e.IsRelease() )
        {
            if( e.GetCode() == VK_SPACE )
            {
                m_bDrawDebugStuff = !m_bDrawDebugStuff;
            }
            else if( e.GetCode() == VK_RETURN )
            {
                restartGame();
            }
            else if( e.GetCode() == VK_ESCAPE )
            {
                wnd.Kill();
            }
            else if( e.GetCode() == 'L' )
            {
                m_bDrawLifeBars = !m_bDrawLifeBars;
            }
        }
    }


    /* keyboard scrolling */
    if( m_bSelecting )
    {
        return;     /* not allowing to scroll while selecting units with rectangle */
    }

    const int pixelsToMove = int( 500.0f * dt );
    if( wnd.kbd.KeyIsPressed( VK_RIGHT ) || wnd.kbd.KeyIsPressed( 'D' ) )
    {
        m_camPos.x += pixelsToMove;
    }
    if( wnd.kbd.KeyIsPressed( VK_LEFT ) || wnd.kbd.KeyIsPressed( 'A' ) )
    {
        m_camPos.x -= pixelsToMove;
    }
    if( wnd.kbd.KeyIsPressed( VK_UP ) || wnd.kbd.KeyIsPressed( 'W' ) )
    {
        m_camPos.y -= pixelsToMove;
    }
    if( wnd.kbd.KeyIsPressed( VK_DOWN ) || wnd.kbd.KeyIsPressed( 'S' ) )
    {
        m_camPos.y += pixelsToMove;
    }
}
bool Game::unitSelected()
{
    for( auto u : m_vpUnits )
    {
        if( u->isSelected() )
        {
            return true;
        }
    }
    return false;
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

    ///////////////////
    //// SEQUENCES ////
    ///////////////////
    auto s = m_vpDeathSequences.begin();
    while( s != m_vpDeathSequences.end() )
    {
        if( ( *s )->update( dt ) )
        {
            delete ( *s );
            s = m_vpDeathSequences.erase( s );
        }
        else
        {
            s++;
        }
    }

    //////////////////
    //// KEYBOARD ////
    //////////////////
    updateKeyboard( dt );

    ////////////////
    //// CURSOR ////
    ////////////////
    m_cursor.update( dt, m_camPos );

    ////////////////
    //// CAMERA ////
    ////////////////
    updateCamera( dt );
}

void Game::drawAllUnits()
{
#if _DEBUG
    const Vei2 halfScreen( Graphics::halfScreenWidth, Graphics::halfScreenHeight );
    const Vei2 offset = m_camPos - halfScreen;
#endif
    /* draw all ground units first */
    for( int i = 0; i < m_vpUnits.size(); ++i )
    {
        if( m_vpUnits[ i ]->isGroundUnit() )
        {
            m_vpUnits[ i ]->draw( gfx, { m_camPos.x, m_camPos.y }, m_bDrawDebugStuff );
#if _DEBUG
            m_font.DrawText( std::to_string( i ), m_vpUnits[ i ]->getLocationInt() - Vei2( 30, 10 ) - offset, Colors::Red, gfx );
#endif
        }
    }
    /* draw life bars on top */
    if( m_bDrawLifeBars )
    {
        for( int i = 0; i < m_vpUnits.size(); ++i )
        {
            if( m_vpUnits[ i ]->isGroundUnit() )
            {
                m_vpUnits[ i ]->drawLifeBar( gfx, m_camPos );
            }
        }
    }

    /* draw all air units */
    for( int i = 0; i < m_vpUnits.size(); ++i )
    {
        if( !m_vpUnits[ i ]->isGroundUnit() )
        {
            m_vpUnits[ i ]->draw( gfx, m_camPos, m_bDrawDebugStuff );
#if _DEBUG
            m_font.DrawText( std::to_string( i ), m_vpUnits[ i ]->getLocationInt() - Vei2( 30, 10 ) - offset, Colors::Red, gfx );
#endif
        }
    }
    /* draw life bars on top */
    if( m_bDrawLifeBars )
    {
        for( int i = 0; i < m_vpUnits.size(); ++i )
        {
            if( !m_vpUnits[ i ]->isGroundUnit() )
            {
                m_vpUnits[ i ]->drawLifeBar( gfx, m_camPos );
            }
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
            // inform all units which have current unit as enemy target
            for( int i = 0; i < m_vpUnits.size(); ++i )
            {
                m_vpUnits[ i ]->checkDestroyedEnemy( *u );
            }

            // add death sequence
            if( ( *u )->getType() == UnitType::TANK || ( *u )->getType() == UnitType::JET )
            {
                m_vpDeathSequences.push_back( new SurfaceSequence( m_explSeqSprite, 14, 1, ( *u )->getLocationInt(), 0.07f ) );
            }

            delete (*u);
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
        bool bMouseOverActionBar = false;
        if( wnd.mouse.GetPosX() > Graphics::ScreenWidth - m_actionBar.getWidth() )
        {
            bMouseOverActionBar = true;
        }

        const Vei2 halfScreen( Graphics::halfScreenWidth, Graphics::halfScreenHeight );
        const Vei2 offset = m_camPos - halfScreen;

        while( !wnd.mouse.IsEmpty() )
        {
            const Mouse::Event e = wnd.mouse.Read();

            if( !bMouseOverActionBar )
            {
                for( auto &u : m_vpUnits )
                {
                    u->handleMouse( e.GetType(), wnd.mouse.GetPos(), m_camPos, wnd.kbd.KeyIsPressed( VK_SHIFT ) );
                }
            }

            /* multi selection rectangle */
            if( e.GetType() == Mouse::Event::Type::LPress )
            {
                if( bMouseOverActionBar )
                {
                    m_selection.left = -1;
                }
                else
                {
                    m_selection.left = wnd.mouse.GetPosX();
                    m_selection.top = wnd.mouse.GetPosY();
                }
            }
            if( e.LeftIsPressed() && m_selection.left >=0 )
            {
                m_bSelecting = true;
                m_selection.bottom = wnd.mouse.GetPosY();
                if( bMouseOverActionBar )
                {
                    m_selection.right = Graphics::ScreenWidth - m_actionBar.getWidth();
                }
                else
                {
                    m_selection.right = wnd.mouse.GetPosX();
                }

                for( auto &u : m_vpUnits )
                {
                    u->handleSelectionRect( m_selection, offset );
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
                    if( m_selection.Contains( u->getLocation() - offset ) )
                    {
                        u->select();
                    }
                }
            }

            /* right-mouse-button-scrolling */
            if( !bMouseOverActionBar && e.RightIsPressed() && !unitSelected() )
            {
                if( !m_bStartedScrolling )
                {
                    m_scrollingStartPos = wnd.mouse.GetPos();
                    m_bStartedScrolling = true;
                }
                m_bScrollingPressed = true;
            }
            else
            {
                m_bScrollingPressed = false;
                m_bStartedScrolling = false;
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
    /* LEVEL */
    m_level.draw( gfx, m_camPos, m_bDrawDebugStuff );
    
    /* UNITS */
    drawAllUnits();

    /* DEATH SEQs */
    for( const auto s : m_vpDeathSequences )
    {
        s->Draw( gfx, m_camPos );
    }

    /* SELECTING RECT */
    if( m_bSelecting )
    {
        gfx.DrawRectBorder( m_selection.getNormalized(), 1, Colors::White );
    }

    /* DEBUG STUFF */
#if _DEBUG  /* display additional unit infos */
    int x = 50;
    char text[ 50 ];
    int numberUnitInfos = std::min( 5, ( int )m_vpUnits.size() );
    for( int i = 0; i < numberUnitInfos; ++i )
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

    /* ACTION BAR */
    m_actionBar.draw( gfx );

    /* SCROLLING RECT */
    if( m_bDrawDebugStuff )
    {
        gfx.DrawRectBorder( m_scrolling_rect, 1, Colors::Magenta );
    }

    /* CURSOR */
    m_cursor.draw( m_camPos, m_bScrollingPressed );
}
