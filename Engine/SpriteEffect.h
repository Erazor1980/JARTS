#pragma once

#include "Colors.h"
#include "Graphics.h"

namespace SpriteEffect
{
    class Chroma
    {
    public:
        Chroma( Color c )
            :
            chroma( c )
        {
        }
        void operator()( Color cSrc, int xDest, int yDest, Graphics& gfx ) const
        {
            if( cSrc != chroma )
            {
                gfx.PutPixel( xDest, yDest, cSrc );
            }
        }
    private:
        Color chroma;
    };
    class Substitution
    {
    public:
        Substitution( Color c, Color s )
            :
            chroma( c ),
            sub( s )
        {
        }
        void operator()( Color cSrc, int xDest, int yDest, Graphics& gfx ) const
        {
            if( cSrc != chroma )
            {
                gfx.PutPixel( xDest, yDest, sub );
            }
        }
    private:
        Color chroma = Colors::Magenta;
        Color sub;
    };
    class Copy
    {
    public:
        void operator()( Color cSrc, int xDest, int yDest, Graphics& gfx ) const
        {
            gfx.PutPixel( xDest, yDest, cSrc );
        }
    };
    class Ghost
    {
    public:
        Ghost( Color c )
            :
            chroma( c )
        {
        }
        void operator()( Color src, int xDest, int yDest, Graphics& gfx ) const
        {
            if( src != chroma )
            {
                const Color dest = gfx.GetPixel( xDest, yDest );
                const Color blend ={
                    unsigned char( ( src.GetR() + dest.GetR() ) / 2 ),
                    unsigned char( ( src.GetG() + dest.GetG() ) / 2 ),
                    unsigned char( ( src.GetB() + dest.GetB() ) / 2 )
                };
                gfx.PutPixel( xDest, yDest, blend );
            }
        }
    private:
        Color chroma;
    };

    class TeamColor
    {
    public:
        TeamColor( Color chroma, Color chromaToSub, Color team )
            :
            chroma( chroma ),
            chromaToSub( chromaToSub ),
            sub( team )
        {
        }
        void operator()( Color cSrc, int xDest, int yDest, Graphics& gfx ) const
        {
            if( cSrc != chroma )
            {
                if( cSrc == chromaToSub )
                {
                    gfx.PutPixel( xDest, yDest, sub );
                }
                else
                {
                    gfx.PutPixel( xDest, yDest, cSrc );
                }
            }
        }
    private:
        Color chroma = Colors::Magenta;
        Color chromaToSub;
        Color sub;
    };
}
