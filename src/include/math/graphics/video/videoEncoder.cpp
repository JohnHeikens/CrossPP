#include "videoEncoder.h"

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
    cint &gridStepcount = frame.size.x / gridStepSize;
    array2d<veci2> motionVectors = array2d<veci2>(veci2(gridStepcount));
    array2d<int> leastErrorPerBlock = array2d<int>(veci2(gridStepcount));
    rectanglei2 blocksRect = rectanglei2(motionVectors.size);
    // the outer borders of the frame will not be checked. too bad
    for (cveci2 &blockPos : blocksRect)
    {
        veci2 posToCheck = (blockPos * gridStepSize) + (gridStepSize / 2);
        ccolor &colorToCompare = totalTexture.getValue(posToCheck);

        constexpr int searchRadius = 0x4;
        veci2 &leastErrorPos = motionVectors.getValueReferenceUnsafe(blockPos);
        int &leastError = leastErrorPerBlock.getValueReferenceUnsafe(blockPos);

        auto checkFunction = [&posToCheck, &colorToCompare, &leastError, &leastErrorPos, &frame](cveci2 &relativePosition)
        {
            ccolor &c = frame.getValue(posToCheck + relativePosition);
            int error = 0;
            if (c != colorToCompare)
            {
                error = 1;
            }
            if (error < leastError)
            {
                leastErrorPos = posToCheck;
            }
            return error == 0;
        };
        veci2 finalCoords;
        if (findInCircles(searchRadius, checkFunction, finalCoords))
        {
        }
        motionWeights[leastErrorPos] += leastError;
    }

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