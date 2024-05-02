#include "math/graphics/texture.h"
#include <map>
#pragma once
typedef array2d<colorRGB> textureRGB;
struct videoEncoder
{
    textureRGB totalTexture = textureRGB(veci2());
    inline void addFrameDiff(const textureRGB &diffTex)
    {
        // temporary, for visualisation
        if (diffTex.size != totalTexture.size || true)
        {
            totalTexture = diffTex;
        }
        else
        {
            const colorChannel *const &totalTexEndPtr = (colorChannel *)totalTexture.end();
            colorChannel *diffPtr = (colorChannel *)diffTex.baseArray;
            for (colorChannel *totalTexPtr = (colorChannel *)totalTexture.baseArray; totalTexPtr < totalTexEndPtr; diffPtr++, totalTexPtr++)
            {
                *totalTexPtr += *diffPtr; // difference can be lower than 0, in which case it will be 255 etc.
            }
        }
    }
    textureRGB addFrame(const textureRGB& frame);
    
};