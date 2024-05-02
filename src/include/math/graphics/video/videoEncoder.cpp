#include "videoEncoder.h"
#include "math/algorithm/findInCircles.h"
#include "math/graphics/brush/brushes.h"

inline constexpr int getColorDifference(const colorRGB &c1, const colorRGB &c2)
{
    return c1[0] > c2[0] ? (c1[0] - c2[0]) : (c2[0] - c1[0]) + c1[1] > c2[1] ? (c1[1] - c2[1])
                                         : (c2[1] - c1[1]) + c1[2] > c2[2]   ? (c1[2] - c2[2])
                                                                             : (c2[2] - c1[2]);
}

// compares the pixels at the moved location in the old frame (moves the old frame 'over' the new frame)
int getMovementCostUnsafe(const textureRGB &oldFrame, const textureRGB &newFrame, crectanglei2 &rectToCheck, cveci2 &movementVector)
{
    int total = 0;

    for (cveci2 &checkPos : rectToCheck)
    {
        total += getColorDifference(oldFrame.getValueUnsafe(checkPos + movementVector), newFrame.getValueUnsafe(checkPos));
    }
    return total;
}

inline void moveArrayPart(const textureRGB &tex, rectanglei2 rect, cveci2 &direction)
{
    if (direction != veci2())
    {
        rectanglei2 texMovedBounds = tex.getClientRect();
        texMovedBounds.pos0 -= direction;
        cint& yStep = tex.size.x;
        if (texMovedBounds.cropClientRect(rect))
        {
            cveci2 &end = rect.pos1();
            if (direction.y < 0)
            {
                // copy from top to bottom
                colorRGB *const &srcYEndPtr = &tex.getValueReferenceUnsafe(rect.pos0);
                colorRGB *srcPtr = srcYEndPtr + yStep * rect.size.y;
                colorRGB *destPtr = srcPtr + direction.x + direction.y * yStep;
                for (; srcPtr > srcYEndPtr;)
                {
                    srcPtr -= yStep;
                    destPtr -= yStep;
                    std::copy(srcPtr, srcPtr + rect.size.x, destPtr);
                }
            }
            else
            {
                const colorRGB *srcPtr = &tex.getValueReferenceUnsafe(rect.pos0);
                const colorRGB *const &srcYEndPtr = srcPtr + yStep * rect.size.y;
                colorRGB *destPtr = &tex.getValueReferenceUnsafe(rect.pos0 + direction);
                cbool &copyForward = direction.y > 0 || direction.x > 0;
                //when copyForward is not satisfied then we copy backwards, because we know that direction.y == 0 && direction.x < 0
                if (!copyForward)
                {
                    destPtr += rect.size.x;
                }
                for (; srcPtr < srcYEndPtr; srcPtr += yStep, destPtr += yStep)
                {
                    if (copyForward)
                    {
                        // copy from left to right, from bottom to top
                        std::copy(srcPtr, srcPtr + rect.size.x, destPtr);
                    }
                    else
                    {
                        // copy from right to left
                        std::copy_backward(srcPtr, srcPtr + rect.size.x, destPtr);
                    }
                }
            }
        }
    }
}

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

    // https://en.wikipedia.org/wiki/Block-matching_algorithm

    // now compare the error of different parts of the image and 'vote' which way to move
    std::map<veci2, int> motionWeights = std::map<veci2, int>();

    // check in a grid
    cint &gridStepSize = 0x40;
    cveci2 &gridSteps = frame.size / gridStepSize;
    array2d<veci2> motionVectors = array2d<veci2>(gridSteps);
    array2d<int> leastErrorPerBlock = array2d<int>(gridSteps);
    rectanglei2 blocksRect = rectanglei2(motionVectors.size);
    // the outer borders of the frame will not be checked. too bad
    for (cveci2 &blockPos : blocksRect)
    {
        veci2 posToCheck = (blockPos * gridStepSize) + (gridStepSize / 2);
        // we extend to the right, but that doesn't really matter
        crectanglei2 &rectToCheck = crectanglei2(posToCheck, cveci2(0x4));
        // ccolor &colorToCompare = totalTexture.getValue(posToCheck);

        constexpr int searchRadius = 0x4;
        veci2 &leastErrorPos = motionVectors.getValueReferenceUnsafe(blockPos);
        int &leastError = leastErrorPerBlock.getValueReferenceUnsafe(blockPos);
        leastError = INT_MAX;

        auto checkFunction = [this, &rectToCheck, &leastError, &leastErrorPos, &frame](cveci2 &relativePosition)
        {
            cint &error = getMovementCostUnsafe(totalTexture, frame, rectToCheck, relativePosition);
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
        motionWeights[leastErrorPos]++; //= leastError;
    }

    int maxVotes = 0;
    veci2 totalMovementVector;
    for (auto p : motionWeights)
    {
        if (p.second > maxVotes)
        {
            totalMovementVector = p.first;
            maxVotes = p.second;
        }
    }

    for (cveci2 &blockPos : blocksRect)
    {
        crectanglei2 &rectToMove = rectanglei2(blockPos * gridStepSize, cveci2(gridStepSize));
        moveArrayPart(totalTexture, rectToMove, motionVectors.getValueUnsafe(blockPos));
    }

    // if (totalMovementVector != veci2())
    //{
    //     textureRGB texCopy = totalTexture;
    //     // move the whole texture
    //     auto movedRect = texCopy.getClientRect();
    //     movedRect.pos0 += totalMovementVector;
    //     fillTransformedBrushRectangle(rectangle2(texCopy.getClientRect()), vec2(totalMovementVector), texCopy, totalTexture);
    // }

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