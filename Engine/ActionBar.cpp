#include "ActionBar.h"
#include "SpriteEffect.h"
#include "RectF.h"

ActionBar::ActionBar( const std::vector< Surface >& vBuildingImages )
    :
    m_vBuildingImages( vBuildingImages ),
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
    m_vRectsInBar_buildings.push_back( RectI( Graphics::ScreenWidth - m_width, Graphics::ScreenWidth - size, startY, startY + size ) );

    /* factory (top right) */
    m_vRectsInBar_buildings.push_back( RectI( Graphics::ScreenWidth - size, Graphics::ScreenWidth, startY, startY + size ) );
}

void ActionBar::draw( Graphics& gfx, Font& font, const Vec2& mousePos )
{
    RectI r( Graphics::ScreenWidth - m_width, Graphics::ScreenWidth, 0, Graphics::ScreenHeight );
    gfx.DrawRect( r, Colors::LightGray );

    /* draw building rects */
    for( auto b : m_vRectsInBar_buildings )
    {
        gfx.DrawRectBorder( b, 2, Colors::Blue );
    }

    /* draw building names/images */
    gfx.DrawSprite( m_vRectsInBar_buildings[ ( int )Building::Type::BARRACKS ].left + 10, m_vRectsInBar_buildings[ ( int )Building::Type::BARRACKS ].top + 10,
                    m_vBuildingImages[ ( int )Building::Type::BARRACKS ], SpriteEffect::Chroma( { 255, 242, 0 } ) );
    gfx.DrawSprite( m_vRectsInBar_buildings[ ( int )Building::Type::FACTORY ].left, m_vRectsInBar_buildings[ ( int )Building::Type::BARRACKS ].top + 10,
                    m_vBuildingImages[ ( int )Building::Type::FACTORY ], SpriteEffect::Chroma( { 255, 242, 0 } ) );
    font.DrawText( "B", m_vRectsInBar_buildings[ ( int )Building::Type::BARRACKS ].GetCenter() - Vei2( m_width / 5, m_width / 5 ), Colors::Blue, gfx );
    font.DrawText( "F", m_vRectsInBar_buildings[ ( int )Building::Type::FACTORY ].GetCenter() - Vei2( m_width / 5, m_width / 5 ), Colors::Blue, gfx );

    /* dummy text */
    font.DrawText( "resources", Vei2( Graphics::ScreenWidth - m_width + 2, 10 ), Colors::Blue, gfx );
    font.DrawText( "other", Vei2( Graphics::ScreenWidth - m_width + 2, 180 ), Colors::Blue, gfx );
    font.DrawText( "buildings", Vei2( Graphics::ScreenWidth - m_width + 2, 210 ), Colors::Blue, gfx );
    font.DrawText( "units", Vei2( Graphics::ScreenWidth - m_width + 2, 280 ), Colors::Blue, gfx );

    /* minimap dummy */
    RectI minimapRect( Graphics::ScreenWidth - m_width, Graphics::ScreenWidth, Graphics::ScreenHeight - 150, Graphics::ScreenHeight );
    gfx.DrawRectBorder( minimapRect, 4, Colors::Yellow );
    font.DrawText( "Minimap", minimapRect.GetCenter() - Vei2( m_width / 3, 10 ), Colors::Blue, gfx );

    /* building placing */
    if( m_bPlacing )
    {
        //const Surface* pCurrentBuilding = &m_vBuildingImages[ ( int )m_buildingType ];

        if( mousePos.x >= Graphics::ScreenWidth - m_width )
        {
            gfx.DrawSprite( ( int )mousePos.x, ( int )mousePos.y, m_vBuildingImages[ ( int )m_buildingType ], SpriteEffect::Ghost( { 255, 242, 0 } ) );
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
                gfx.DrawSprite( ( int )m_vBuildingTiles.front().left, ( int )m_vBuildingTiles.front().top, 
                                m_vBuildingImages[ ( int )m_buildingType ], SpriteEffect::Chroma( { 255, 242, 0 } ) );
            }
            else
            {
                gfx.DrawSprite( ( int )m_vBuildingTiles.front().left, ( int )m_vBuildingTiles.front().top,
                                m_vBuildingImages[ ( int )m_buildingType ], SpriteEffect::Ghost( { 255, 242, 0 } ) );
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
        m_vBuildingIndices.clear();

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
                int idx     = level.getTileIdx( ( int )mousePos.x + offset.x + x * ( int )tileSize, ( int )mousePos.y + offset.y + y * ( int )tileSize );

                m_vBuildingIndices.push_back( idx );
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

void ActionBar::handleMouse( const Mouse::Event::Type& type, const Vec2& mousePos, std::vector< Building >& vBuildings )
{
    if( type == Mouse::Event::Type::LPress )
    {
        /* inside action bar */
        if( mousePos.x > Graphics::ScreenWidth - m_width )
        {
            /* buildings */
            if( mousePos.y < m_vRectsInBar_buildings.back().bottom )
            {
                for( int i = 0; i < m_vRectsInBar_buildings.size(); ++i )
                {
                    if( m_vRectsInBar_buildings[ i ].Contains( mousePos ) )
                    {
                        m_buildingType  = Building::Type( i );
                        m_bPlacing      = true;
                        m_buildingSize  = Building::getSizeInTiles( m_buildingType );
                        return;
                    }
                }
            }
            /* units */
            //else if( mousePos.y < m_vRectsInBar_units.back().bottom )
            //{
                //TODO add later
            //}
        }
        /* outside action bar */
        else
        {
            /* place building */
            if( m_bPlacing && m_bFreeSpace )
            {
                //vBuildings.push_back( Building( m_buildingType, m_vBuildingIndices ) );
                m_bPlacing = false;
            }
        }

    }
    if( m_bPlacing )
    {
        if( type == Mouse::Event::Type::RPress )
        {
            m_bPlacing = false;
        }
    }
}
