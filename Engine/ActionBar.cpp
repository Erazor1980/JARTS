#include "ActionBar.h"
#include "SpriteEffect.h"
#include "RectF.h"

ActionBar::ActionBar()
    :
    m_factoryImg( "..\\images\\actionBar\\factory.bmp" ),
    m_barracksImg( "..\\images\\actionBar\\barracks.bmp" ),
#if _DEBUG
    m_img( "..\\images\\debugImg.bmp" )
#else
    m_img( "..\\images\\actionBar\\actionBar.bmp" )
#endif
{
#if _DEBUG
    m_width = 200;
#else
    m_width = m_img.GetWidth();
#endif
    const int size      = m_width / 2;
    const int startY    = 50;
    /* barracks (top left) */
    m_vBuildingRects.push_back( RectI( Graphics::ScreenWidth - m_width, Graphics::ScreenWidth - size, startY, startY + size ) );

    /* factory (top right) */
    m_vBuildingRects.push_back( RectI( Graphics::ScreenWidth - size, Graphics::ScreenWidth, startY, startY + size ) );
}

void ActionBar::draw( Graphics& gfx, Font& font, const Vec2& mousePos )
{
#if _DEBUG
    RectI r( Graphics::ScreenWidth - m_width, Graphics::ScreenWidth, 0, Graphics::ScreenHeight );
    gfx.DrawRectBorder( r, 4, Colors::LightGray );
#else
    gfx.DrawSprite( Graphics::ScreenWidth - m_width, 0, m_img, SpriteEffect::Copy{} );
#endif

    /* draw building rects */
    for( auto b : m_vBuildingRects )
    {
        gfx.DrawRectBorder( b, 2, Colors::Blue );
    }

    /* draw building names */
    font.DrawText( "B", m_vBuildingRects[ ( int )Building::Type::BARRACKS ].GetCenter() - Vei2( 8, 14 ), Colors::Blue, gfx );
    font.DrawText( "F", m_vBuildingRects[ ( int )Building::Type::FACTORY ].GetCenter() - Vei2( 8, 14 ), Colors::Blue, gfx );

    /* building placing */
    if( m_bPlacing )
    {
        Surface* pCurrentBuilding = nullptr;
        switch( m_buildingType )
        {
        case Building::Type::BARRACKS:
            pCurrentBuilding = &m_barracksImg;
            break;
        case Building::Type::FACTORY:
            pCurrentBuilding = &m_factoryImg;
            break;
        default:
            pCurrentBuilding = nullptr;
        }

        if( mousePos.x >= Graphics::ScreenWidth - m_width )
        {
            gfx.DrawSprite( ( int )mousePos.x, ( int )mousePos.y, *pCurrentBuilding, SpriteEffect::Ghost( { 255, 242, 0 } ) );
        }
        else
        {
            for( int i = 0; i < m_vBuildingTiles.size(); ++i )
            {
                if( m_vTileOccupancy[ i ] )
                {
                    gfx.DrawRectBorder( m_vBuildingTiles[ i ], 2, Colors::Green );
                }
                else
                {
                    gfx.DrawRectBorder( m_vBuildingTiles[ i ], 2, Colors::Red );
                }
            }
            if( m_bFreeSpace )
            {
                gfx.DrawSprite( ( int )m_vBuildingTiles.front().left, ( int )m_vBuildingTiles.front().top, *pCurrentBuilding, SpriteEffect::Chroma( { 255, 242, 0 } ) );
            }
            else
            {
                gfx.DrawSprite( ( int )m_vBuildingTiles.front().left, ( int )m_vBuildingTiles.front().top, *pCurrentBuilding, SpriteEffect::Ghost( { 255, 242, 0 } ) );
            }
        }        
    }
}

void ActionBar::update( const float dt, const Vec2& mousePos, const Vei2& camPos, const Level& level )
{
    if( m_bPlacing )
    {
        m_vBuildingTiles.clear();
        m_vTileOccupancy.clear();

        const Vei2 halfScreen( Graphics::halfScreenWidth, Graphics::halfScreenHeight );
        const Vei2 offset       = camPos - halfScreen;

        const float tileSize    = ( float )level.getTileSize();
        m_bFreeSpace            = true;
        for( int x = 0; x < m_buildingSize.x; ++x )
        {
            for( int y = 0; y < m_buildingSize.y; ++y )
            {
                RectF r     = level.getTileRect( mousePos + Vec2( x * tileSize, y * tileSize ), camPos );
                Tile tile   = level.getTileType( ( int )mousePos.x + offset.x + x * ( int )tileSize, ( int )mousePos.y + offset.y + y * ( int )tileSize );

                m_vBuildingTiles.push_back( r );
                if( Tile::EMPTY == tile )
                {
                    m_vTileOccupancy.push_back( true );
                    
                }
                else
                {
                    m_vTileOccupancy.push_back( false );
                    m_bFreeSpace = false;
                }
            }
        }
    }
}

void ActionBar::handleMouse( const Mouse::Event::Type& type, const Vec2& mousePos )
{
    /* buildings */
    if( mousePos.y < m_vBuildingRects.back().bottom )
    {
        if( type == Mouse::Event::Type::LPress )
        {
            for( int i = 0; i < m_vBuildingRects.size(); ++i )
            {
                if( m_vBuildingRects[ i ].Contains( mousePos ) )
                {
                    m_buildingType  = Building::Type( i );
                    m_bPlacing      = true;
                    m_buildingSize  = Building::getSizeInTiles( m_buildingType );
                    return;
                }
            }
            m_bPlacing = false;
        }
    }
}
