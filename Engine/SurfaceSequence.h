#pragma once
#include <string>
#include <vector>
#include <cassert>

#include "Surface.h"
#include "Graphics.h"
#include "RectI.h"
#include "SpriteEffect.h"

class SurfaceSequence
{
public:
    SurfaceSequence( const Surface& seqSprite, const int imagesPerRow, const int imagesPerColumn, const Vei2& fixPos = { 0, 0 },
                     float holdTime = 0.025f, Color chroma = Colors::White )
        :
        m_sprite( seqSprite ),
        m_nImgRows( imagesPerRow ),
        m_nImgCols( imagesPerColumn ),
        m_holdTime( holdTime ),
        m_chroma( chroma ),
        m_pos( fixPos )
    {
        assert( imagesPerRow >= 1 && imagesPerColumn >= 1 && holdTime > 0.0f );
        const int subImgWidth = m_sprite.GetWidth() / imagesPerRow;
        const int subImgHeight = m_sprite.GetHeight() / imagesPerColumn;
        assert( m_sprite.GetWidth() % subImgWidth == 0 );
        assert( m_sprite.GetHeight() % subImgHeight == 0 );

        m_halfWidth     = subImgWidth / 2;
        m_halfHeight    = subImgHeight / 2;

        /* calculating rectangles for every image of the sequence */
        for( int y = 0; y < imagesPerColumn; ++y )
        {
            for( int x = 0; x < imagesPerRow; ++x )
            {
                RectI r( { x * subImgWidth, y * subImgHeight }, subImgWidth, subImgHeight );
                m_vSpriteRects.emplace_back( r );
            }            
        }
    }

    void Draw( Graphics& gfx, int x = -100, int y = -100 ) const
    {
        if( x != -100 && y != -100 )
        {
            gfx.DrawSprite( x - m_halfWidth, y - m_halfHeight, m_vSpriteRects[ m_iCurSurface ], m_sprite, SpriteEffect::Chroma( m_chroma ) );
        }
        else
        {
            gfx.DrawSprite( m_pos.x - m_halfWidth, m_pos.y - m_halfHeight, m_vSpriteRects[ m_iCurSurface ], m_sprite, SpriteEffect::Chroma( m_chroma ) );
        }        
    }
    bool update( const float dt )       /* returns true when sequence is over */
    {
        m_time += dt;
        if( m_time >= m_holdTime )
        {
            m_iCurSurface++;
            //m_iCurSurface %= m_vSpriteRects.size();
            m_time = 0.0f;
        }

        if( m_iCurSurface == m_vSpriteRects.size() )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
private:
    const Surface& m_sprite;
    const unsigned int m_nImgRows;          /* number of images per row */
    const unsigned int m_nImgCols;          /* number of images per column */
    const Color m_chroma;
    const float m_holdTime;                 /* time in seconds for one image of the sequence */
    float m_time = 0.0f;                    /* holding time of the current image */
    unsigned int m_iCurSurface = 0;         /* current image index */
    std::vector< RectI > m_vSpriteRects;

    const Vei2 m_pos;                       /* if draw is called without x and y, this fix position will be used (set in constructor) */
    
    /* for drawing sprite in the center (and not top left) */
    int m_halfWidth;
    int m_halfHeight;
};