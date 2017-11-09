#include "Level.h"
#include "SpriteEffect.h"
#include <assert.h>
#include <algorithm>

Level::Level( const std::string& filename, const int actionBarWidth )
    :
    m_lvlImg( filename )
{
    m_actionBarWidth = actionBarWidth;
    init();
}

Level::~Level()
{
    if( mp_content )
    {
        delete[] mp_content;
        mp_content = nullptr;
    }
}

void Level::init()
{
#if _DEBUG
    m_widthInTiles  = 1600 / m_tileSize;
    m_heightInTiles = 800 / m_tileSize;
    m_width         = 1600;
    m_height        = 800;
#else
    m_widthInTiles      = m_lvlImg.GetWidth() / m_tileSize;
    m_heightInTiles     = m_lvlImg.GetHeight() / m_tileSize;
    m_width             = m_lvlImg.GetWidth();
    m_height            = m_lvlImg.GetHeight();
#endif

    m_halfWidth     = m_width / 2;
    m_halfHeight    = m_height / 2;

    assert( m_width > 0 && m_height > 0 && m_heightInTiles > 0 && m_widthInTiles > 0 );
    
    if( mp_content )
    {
        delete[] mp_content;
    }
    mp_content = new Tile[ m_widthInTiles * m_heightInTiles ];

    /* random level */
    for( int i = 0; i < m_widthInTiles * m_heightInTiles; ++i )
    {
        if( rand() % 4 )
        {
            mp_content[ i ] = Tile::EMPTY;
        }
        else
        {
            mp_content[ i ] = Tile::OBSTACLE;
        }
    }

    m_bInitialized = true;
}

Vec2 Level::getTileCenter( const int tileIdx ) const
{
    const int xTile = tileIdx % m_widthInTiles;
    const int yTile = tileIdx / m_widthInTiles;

    const float x = xTile * m_tileSize + m_tileSize / 2.0f - 1;
    const float y = yTile * m_tileSize + m_tileSize / 2.0f - 1;

    return Vec2( x, y );
}

void Level::draw( Graphics& gfx, const Vei2& camera, const bool drawGrid ) const
{
    /* calculate RectI of the current visible map snippet */
    const int xStart = std::max( camera.x - Graphics::halfScreenWidth, 0 );
    const int yStart = std::max( camera.y - Graphics::halfScreenHeight, 0 );
    const int xEnd = std::min( m_width, camera.x + Graphics::halfScreenWidth - m_actionBarWidth );
    const int yEnd = std::min( m_height, camera.y + Graphics::halfScreenHeight );

    RectI snippet = RectI( xStart, xEnd, yStart, yEnd );

#if !_DEBUG
    gfx.DrawSprite( 0, 0, snippet, m_lvlImg, SpriteEffect::Copy{} );
#endif

    /* tile grid */
    if( drawGrid )
    {
        drawTileGrid( gfx, camera, false );
    }    
}

void Level::addBuilding( std::vector< int > vTileIndices )
{
    const int n = m_widthInTiles * m_heightInTiles;
    for( const auto i : vTileIndices )
    {
        if( i > 0 && i < n )
        {
            mp_content[ i ] = Tile::BUILDING;
        }
    }
}

void Level::removeBuilding( std::vector< int > vTileIndices )
{
    const int n = m_widthInTiles * m_heightInTiles;
    for( const auto i : vTileIndices )
    {
        if( i > 0 && i < n )
        {
            mp_content[ i ] = Tile::EMPTY;
        }
    }
}

void Level::drawTileGrid( Graphics& gfx, const Vei2& camera, const bool drawFreeTiles ) const
{
    assert( m_height > 0 && m_width > 0 && m_tileSize > 0 );

    const int xOffset = camera.x - Graphics::halfScreenWidth;
    const int yOffset = camera.y - Graphics::halfScreenHeight;

    for( int x = 0; x < m_widthInTiles; ++x )
    {
        int leftX = x * m_tileSize - xOffset;
        if( leftX > Graphics::ScreenWidth - m_actionBarWidth )
        {
            continue;
        }
        for( int y = 0; y < m_heightInTiles; ++y )
        {
            RectI tile( { leftX, y * m_tileSize - yOffset },
                        { ( x + 1 ) * m_tileSize - 1 - xOffset, ( y + 1 ) * m_tileSize - 1 - yOffset } );

            if( Tile::OBSTACLE == mp_content[ y * m_widthInTiles + x ] )
            {
                gfx.DrawRectBorder( tile, 1, Colors::Red );
                Vei2 tl = { tile.left, tile.top };
                Vei2 br = { tile.right, tile.bottom };
                gfx.DrawLine( tl, br, Colors::Red );
            }
            else if( drawFreeTiles )
            {
                gfx.DrawRectBorder( tile, 1, Colors::Green );
            }
        }
    }
}

void Level::drawPath( Graphics& gfx, const Vei2& camera, std::vector< int > vPath, const int currIdx, const int startIdx, const int targetIdx ) const
{
    /* draw path */
    for( int i = currIdx; i < vPath.size(); ++i )
    {
        const int x = vPath[ i ] % m_widthInTiles;
        const int y = vPath[ i ] / m_widthInTiles;

        RectI tile( { x * m_tileSize - ( int )camera.x, y * m_tileSize - ( int )camera.y }, { ( x + 1 ) * m_tileSize - 1, ( y + 1 ) * m_tileSize - 1 } );

        gfx.DrawCircle( tile.GetCenter().x, tile.GetCenter().y, 4, Colors::White );
    }

    /* draw start and target */
    if( startIdx >= 0 && targetIdx >= 0 )
    {
        const int xS = startIdx % m_widthInTiles;
        const int yS = startIdx / m_widthInTiles;
        RectI start( { xS * m_tileSize, yS * m_tileSize }, { ( xS + 1 ) * m_tileSize - 1, ( yS + 1 ) * m_tileSize - 1 } );
        gfx.DrawRectBorder( start, 2, Colors::Red );

        const int xT = targetIdx % m_widthInTiles;
        const int yT = targetIdx / m_widthInTiles;
        RectI end( { xT * m_tileSize, yT * m_tileSize }, { ( xT + 1 ) * m_tileSize - 1, ( yT + 1 ) * m_tileSize - 1 } );
        gfx.DrawRectBorder( end, 2, Colors::Blue );
    }
}
