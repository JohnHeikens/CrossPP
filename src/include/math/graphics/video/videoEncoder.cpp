#include "videoEncoder.h"
#include "math/algorithm/findInCircles.h"
#include "math/graphics/brush/brushes.h"
textureRGB videoEncoder::addFrame(const textureRGB &frame)
{
    if (frame.size != totalTexture.size)
    {
        totalTexture = frame;
        return textureRGB(frame);
    }
    // TODO: remove unnecessary copy of frame to totaltexture
    const textureRGB &diffTex = textureRGB(frame.size, false);

    // optimize frame further: detect general image motion

    // now compare the error of different parts of the image and 'vote' which way to move
    std::map<veci2, int> motionWeights = std::map<veci2, int>();

    // check in a grid
    cint &gridStepSize = 0x200;
    cveci2 &gridSteps = frame.size / gridStepSize;
    array2d<veci2> motionVectors = array2d<veci2>(gridSteps);
    array2d<int> leastErrorPerBlock = array2d<int>(gridSteps);
    rectanglei2 blocksRect = rectanglei2(motionVectors.size);
    // the outer borders of the frame will not be checked. too bad
    for (cveci2 &blockPos : blocksRect)
    {
        veci2 posToCheck = (blockPos * gridStepSize) + (gridStepSize / 2);
        ccolor &colorToCompare = totalTexture.getValue(posToCheck);

        constexpr int searchRadius = 0x4;
        veci2 &leastErrorPos = motionVectors.getValueReferenceUnsafe(blockPos);
        int &leastError = leastErrorPerBlock.getValueReferenceUnsafe(blockPos);
        leastError = INT_MAX;

        auto checkFunction = [&posToCheck, &colorToCompare, &leastError, &leastErrorPos, &frame](cveci2 &relativePosition)
        {
            ccolor &c = frame.getValue(posToCheck + relativePosition);
            cint& error = 
            c[0] > colorToCompare[0] ? (c[0] - colorToCompare[0]) : (colorToCompare[0] - c[0]) +
            c[1] > colorToCompare[1] ? (c[1] - colorToCompare[1]) : (colorToCompare[1] - c[1]) +
            c[2] > colorToCompare[2] ? (c[2] - colorToCompare[2]) : (colorToCompare[2] - c[2]);
            if (error < leastError)
            {
                leastErrorPos = relativePosition;
                leastError = error;
            }
            return error == 0;
        };
        veci2 finalCoords;
        if (findInCircles(searchRadius, checkFunction, finalCoords))
        {
        }
        motionWeights[leastErrorPos] += leastError;
    }

    int maxVotes = 0;
    veci2 movementVector;
    for(auto p : motionWeights){
        if(p.second > maxVotes){
            movementVector = p.first;
        }
    }
    textureRGB texCopy = totalTexture;
    //move the whole texture
    auto movedRect = texCopy.getClientRect();
    movedRect.pos0 += movementVector;
    fillTransformedBrushRectangle(rectangle2(texCopy.getClientRect()), vec2(movementVector), texCopy, totalTexture);

    const colorChannel *const &totalTexEndPtr = (colorChannel *)totalTexture.end();
    colorChannel *diffPtr = (colorChannel *)diffTex.baseArray;
    colorChannel *framePtr = (colorChannel *)frame.baseArray;
    for (colorChannel *totalTexPtr = (colorChannel *)totalTexture.baseArray; totalTexPtr < totalTexEndPtr; framePtr++, diffPtr++, totalTexPtr++)
    {
        *diffPtr = *framePtr - *totalTexPtr; // difference can be lower than 0, in which case it will be 255 etc.
        *totalTexPtr = *framePtr;
    }
    return diffTex;
}