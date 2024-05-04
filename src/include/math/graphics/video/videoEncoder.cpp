#include "videoEncoder.h"
#include "math/algorithm/findInCircles.h"
#include "math/graphics/brush/brushes.h"
#include <map>
#include "nbtSerializer.h"
#include "gzipIncluder.h"

constexpr int gridStepSize = 0x20;

inline constexpr int getByteDifference(cbyte &b1, cbyte &b2)
{
    return b1 > b2 ? b1 - b2 : b2 - b1;
}

inline constexpr int getColorDifference(const colorRGB &c1, const colorRGB &c2)
{
    return getByteDifference(c1[0], c2[0]) +
           getByteDifference(c1[1], c2[1]) +
           getByteDifference(c1[2], c2[2]);
}

// crop a rectangle based on direction and the image
// the rect should be inside of the bounds, but the rect + movementvector should also be inside of the bounds
inline bool cropCheckRect(rectanglei2 bounds, rectanglei2 &rect, cveci2 &movementVector)
{
    bounds.pos0 -= movementVector;
    crectanglei2(bounds.pos0 - movementVector, bounds.size).cropClientRect(bounds);
    return bounds.cropClientRect(rect);
}

// compares the pixels of the old location in the old frame to
// the pixels of the moved location in the new frame
// (moves the old frame 'over' the new frame)
inline int getMovementCostUnsafe(const textureRGB &oldFrame, const textureRGB &newFrame, rectanglei2 rectToCheck, cveci2 &movementVector, cint &maxError)
{
    int total = 0;
    //check one pixel of every #x# where # = checkInterval
    constexpr int checkInterval = 0x4;
    if (cropCheckRect(oldFrame.getClientRect(), rectToCheck, movementVector))
    {
        crectanglei2 &baseRect = rectanglei2(veci2(), rectToCheck.size / checkInterval);
        for (cveci2 &baseCheckPos : baseRect)
        {
            cveci2 &checkPos = rectToCheck.pos0 + (baseCheckPos * checkInterval);
            total += getColorDifference(oldFrame.getValueUnsafe(checkPos), newFrame.getValueUnsafe(checkPos + movementVector));
            if (total >= maxError)
            {
                return total;
            }
        }
        return total;
    }
    return INT_MAX;
}

inline void moveArrayPart(const textureRGB &tex, rectanglei2 rect, cveci2 &direction)
{
    if (direction != veci2())
    {
        if (cropCheckRect(tex.getClientRect(), rect, direction))
        {
            // copy in the reverse order of moving
            cint &yStep = tex.size.x;
            cveci2 &end = rect.pos1();
            if (direction.y > 0)
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
                cbool &copyForward = direction.y < 0 || direction.x < 0;
                // when copyForward is not satisfied then we copy backwards, because we know that direction.y == 0 && direction.x > 0
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

void videoEncoder::addFrameDiff(const textureRGB &diffTex, nbtSerializer &serializer)
{
    //  temporary, for visualisation
    constexpr bool showDebugInfo = true;
    if (diffTex.size != totalTexture.size || showDebugInfo)
    {
        totalTexture = diffTex;
        resizeGrid();
        if constexpr (showDebugInfo)
        {
            serializeMotionVectors(serializer);
        }
    }
    else
    {

        serializeMotionVectors(serializer);
        addMotionVectors();
        const colorChannel *const &totalTexEndPtr = (colorChannel *)totalTexture.end();
        colorChannel *diffPtr = (colorChannel *)diffTex.baseArray;
        for (colorChannel *totalTexPtr = (colorChannel *)totalTexture.baseArray; totalTexPtr < totalTexEndPtr; diffPtr++, totalTexPtr++)
        {
            *totalTexPtr += *diffPtr; // difference can be lower than 0, in which case it will be 255 etc.
        }
    }
}

textureRGB videoEncoder::addFrame(const textureRGB &frame)
{
    if (frame.size != totalTexture.size)
    {
        totalTexture = frame;
        resizeGrid();
        return textureRGB(frame);
    }
    // TODO: remove unnecessary copy of frame to totaltexture
    const textureRGB &diffTex = textureRGB(frame.size, false);

    // optimize frame further: detect general image motion

    // https://en.wikipedia.org/wiki/Block-matching_algorithm

    // now compare the error of different parts of the image and 'vote' which way to move
    std::map<veci2, int> motionWeights = std::map<veci2, int>();

    // check in a grid
    const array2d<int> leastErrorPerBlock = array2d<int>(motionVectors.size);
    crectanglei2 &blocksRect = rectanglei2(motionVectors.size);
    // the outer borders of the frame will not be checked. too bad
    for (cveci2 &blockPos : blocksRect)
    {
        //veci2 posToCheck = (blockPos * gridStepSize) + (gridStepSize / 2);
        // we extend to the right, but that doesn't really matter
        //crectanglei2 &rectToCheck = crectanglei2(posToCheck - cveci2(0x4), cveci2(0x8));
        crectanglei2 &rectToCheck = crectanglei2(blockPos * gridStepSize, cveci2(gridStepSize));
        // ccolor &colorToCompare = totalTexture.getValue(posToCheck);

        constexpr int searchRadius = 0x8;
        vect2<sbyte> &leastErrorPos = motionVectors.getValueReferenceUnsafe(blockPos);
        int &leastError = leastErrorPerBlock.getValueReferenceUnsafe(blockPos);

        // copy
        cveci2 lastMotion = leastErrorPos;

        // leastError = INT_MAX;
        std::map<veci2, int> blockMotionWeight = std::map<veci2, int>();
        // first check 00, then check around the old movement, to create constant motion. constant motion is not appreciated when there's a still part of an image.

        leastErrorPos = veci2();
        leastError = getMovementCostUnsafe(totalTexture, frame, rectToCheck, leastErrorPos, INT_MAX);
        blockMotionWeight[leastErrorPos] = leastError;

        if (leastError)
        {
            const auto &checkFunction = [this, &lastMotion, &blockMotionWeight, &rectToCheck, &leastError, &leastErrorPos, &frame](cveci2 &circlePosition)
            {
                cveci2 &relativePosition = lastMotion + circlePosition;
                if (blockMotionWeight.contains(relativePosition))
                {
                    return false;
                }
                cint &error = getMovementCostUnsafe(totalTexture, frame, rectToCheck, relativePosition, leastError);
                blockMotionWeight[relativePosition] = error;
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
            // old: // finally, check 00 if it hasn't been checked yet
            //      if (leastError && !blockMotionWeight.contains(veci2()))
            //      {
            //          cint &error = getMovementCostUnsafe(totalTexture, frame, rectToCheck, leastErrorPos, leastError);
            //          if (error < leastError)
            //          {
            //              leastErrorPos = veci2();
            //              leastError = error;
            //          }
            //      }

            motionWeights[leastErrorPos]++; //= leastError;
        }
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
    addMotionVectors();

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

void videoEncoder::serializeMotionVectors(nbtSerializer &serializer)
{
    std::string compressedVectors;
    if (serializer.write)
    {
        compressedVectors = gzip::compress((const char *)motionVectors.baseArray, motionVectors.size.volume() * sizeof(vect2<sbyte>));
    }
    // serialize the motion vectors as one big int array
    serializer.serializeValue(L"motion vectors", compressedVectors);

    if (!serializer.write)
    {
        std::string decomp = gzip::decompress(&compressedVectors[0], compressedVectors.size());
        std::copy(decomp.begin(), decomp.end(), (char *)motionVectors.baseArray);
    }
}

void videoEncoder::visualize(const texture &screen)
{

    // visualize the moved rects
    crectanglei2 &blocksRect = rectanglei2(motionVectors.size);
    for (cveci2 &blockPos : blocksRect)
    {
        crectanglei2 &rectToMove = rectanglei2(blockPos * gridStepSize, cveci2(gridStepSize));
        cveci2 &vec = motionVectors.getValueUnsafe(blockPos);
        if (vec != veci2())
        {
            screen.fillRectangleBorders(rectToMove, 1, brushes::red);
            constexpr int motionMult = 30;
            screen.fillLine(rectToMove.pos0, rectToMove.pos0 + vec * motionMult, brushes::green);
        }
    }
}

void videoEncoder::addMotionVectors() const
{
    for (cveci2 &blockPos : motionVectors.getClientRect())
    {
        crectanglei2 &rectToMove = rectanglei2(blockPos * gridStepSize, cveci2(gridStepSize));
        moveArrayPart(totalTexture, rectToMove, motionVectors.getValueUnsafe(blockPos));
    }
}

void videoEncoder::resizeGrid()
{
    motionVectors = array2d<vect2<sbyte>>(totalTexture.size / gridStepSize);
}