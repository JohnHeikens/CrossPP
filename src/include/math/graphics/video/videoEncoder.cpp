#include "videoEncoder.h"
#include "math/algorithm/findInCircles.h"
#include "math/graphics/brush/brushes.h"
#include <map>
#include "nbtSerializer.h"
#include "gzipIncluder.h"
#include "serializer/serializeList.h"

constexpr int gridStepSize = 0x20;

constexpr colorRGB nothingChangedColor = colorRGB(0x79);

int marginPerColor = 0x10; // the full colors may differ more than

inline constexpr int getByteDifference(cbyte &b1, cbyte &b2)
{
    return b1 > b2 ? b1 - b2 : b2 - b1;
}

inline constexpr bool isRoughlyTheSameColorValue(cbyte &currentByte, cbyte &targetByte)
{
    // exponential: the higher the byte value, the less one will notice a tad bit difference.
    // our brains measure impulses exponentially
    return getByteDifference(currentByte, targetByte) <= 0x8;
}

constexpr bool isRoughlyTheSameColor(const colorRGB &currentColor, const colorRGB &targetColor)
{
    // nice for the future: bit comparison for speedup
    constexpr int channelCompareValue = 0b11110000;
    constexpr int colorCompareValue = channelCompareValue + channelCompareValue * 0x100 + channelCompareValue * 0x10000;

    return isRoughlyTheSameColorValue(currentColor[0], targetColor[0]) &&
           isRoughlyTheSameColorValue(currentColor[1], targetColor[1]) &&
           isRoughlyTheSameColorValue(currentColor[2], targetColor[2]);
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
    //bounds.pos0 -= movementVector;
    return crectanglei2(bounds.pos0 - movementVector, bounds.size).cropClientRect(bounds) &&
           bounds.cropClientRect(rect);
}

// compares the pixels of the old location in the old frame to
// the pixels of the moved location in the new frame
// (moves the old frame 'over' the new frame)
inline int getMovementCostUnsafe(const textureRGB &oldFrame, const textureRGB &newFrame, rectanglei2 rectToCheck, cveci2 &movementVector, cint &checkInterval, cint &maxError)
{
    int total = 0;
    // check one pixel of every #x# where # = checkInterval
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

        const colorRGB *const &totalTexEndPtr = (colorRGB *)totalTexture.end();
        colorRGB *diffPtr = (colorRGB *)diffTex.baseArray;
        for (colorRGB *totalTexPtr = (colorRGB *)totalTexture.baseArray; totalTexPtr < totalTexEndPtr; diffPtr++, totalTexPtr++)
        {
            if (*diffPtr != nothingChangedColor)
            {
                *totalTexPtr = *diffPtr;
            }
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

    // block matching algorithms don't seem to help much:
    // https://en.wikipedia.org/wiki/Block-matching_algorithm
    // we'll store the motion of each pixel and make an image of this motion. the PNG compressor will create a PNG from the motion.

    // now compare the error of different parts of the image and 'vote' which way to move
    std::map<veci2, int> motionWeights = std::map<veci2, int>();

    crectanglei2 &imageRect = frame.getClientRect();

    // find rectangles that are moving

    // std::vector<rectanglei2, veci2> motionRects = std::vector<rectanglei2, veci2>(frame.size);

    // get motion vectors for the whole image

    constexpr byte motionCheckStepCount = 2;
    constexpr int motionBlockSize[motionCheckStepCount] = {
        0x100,
        0x10};
    constexpr int checkInterval[motionCheckStepCount] = {
        0x20,
        0x4};
    constexpr int startingS[motionCheckStepCount] = {
        0x10,
        0x4};

    array2d<veci2> motionVectors[motionCheckStepCount];

    // resize arrays
    for (byte motionStepIndex = 0; motionStepIndex < motionCheckStepCount; motionStepIndex++)
    {
        motionVectors[motionStepIndex] = array2d<veci2>(ceilVector(vec2(frame.size) / (fp)motionBlockSize[motionStepIndex]));
    }

    for (byte motionStepIndex = 0; motionStepIndex < motionCheckStepCount; motionStepIndex++)
    {
        const array2d<veci2> &currentVectorArray = motionVectors[motionStepIndex];

        crectanglei2 &blocksRect = rectanglei2(currentVectorArray.size);
        // the outer borders of the frame will not be checked. too bad
        for (cveci2 &blockPos : blocksRect)
        {
            // veci2 posToCheck = (blockPos * gridStepSize) + (gridStepSize / 2);
            //  we extend to the right, but that doesn't really matter
            // crectanglei2 &rectToCheck = crectanglei2(posToCheck - cveci2(0x4), cveci2(0x8));
            crectanglei2 &rectToCheck = crectanglei2(blockPos * gridStepSize, cveci2(gridStepSize));
            // ccolor &colorToCompare = totalTexture.getValue(posToCheck);

            // constexpr int searchRadius = 0x8;
            veci2 &leastErrorPos = currentVectorArray.getValueReferenceUnsafe(blockPos);
            // int leastError = INT_MAX;

            // copy
            cveci2 lastMotion = leastErrorPos;

            // leastError = INT_MAX;
            std::map<veci2, int> blockMotionWeight = std::map<veci2, int>();
            // first check 00, then check around the old movement, to create constant motion. constant motion is not appreciated when there's a still part of an image.

            leastErrorPos = veci2();
            int leastError = getMovementCostUnsafe(totalTexture, frame, rectToCheck, leastErrorPos, checkInterval[motionStepIndex], INT_MAX);
            blockMotionWeight[leastErrorPos] = leastError;

            if (leastError)
            {
                // copy
                // we will be zooming in on this position (TSS)
                veci2 zoomInOn = lastMotion;
                for (int stepSize = startingS[motionStepIndex]; stepSize; stepSize /= 2, zoomInOn = leastErrorPos)
                {
                    const auto &checkFunction = [this, &zoomInOn, &blockMotionWeight, &rectToCheck, &leastError, &leastErrorPos, &motionStepIndex, &frame, &checkInterval, stepSize](cveci2 &circlePosition)
                    {
                        cveci2 &relativePosition = zoomInOn + circlePosition * stepSize;
                        if (blockMotionWeight.contains(relativePosition))
                        {
                            return false;
                        }
                        cint &error = getMovementCostUnsafe(totalTexture, frame, rectToCheck, relativePosition, checkInterval[motionStepIndex], leastError);
                        blockMotionWeight[relativePosition] = error;
                        if (error < leastError)
                        {
                            leastErrorPos = relativePosition;
                            leastError = error;
                        }
                        return error == 0;
                    };
                    veci2 finalCoords;
                    if (findInCircles(2, checkFunction, finalCoords))
                    {
                        // exact match
                        break;
                    }
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
    }

    const auto &lastMotionVectorArray = motionVectors[motionCheckStepCount - 1];
    constexpr int lastMotionBlockSize = motionBlockSize[motionCheckStepCount - 1];
    for (cveci2 &pixelPos : imageRect)
    {
        // first check 0 0
        const colorRGB &currentSearchColor = frame.getValueUnsafe(pixelPos);
        if (isRoughlyTheSameColor(totalTexture.getValueUnsafe(pixelPos), currentSearchColor))
        {
            sourceMotionVectors.setValueUnsafe(pixelPos, compressedSourceMotionVector());
        }
        else
        {
            // check last motion vector
            auto lastSourceMotionVector = sourceMotionVectors.getValueUnsafe(pixelPos);
            cveci2 &lastOffsetPos = pixelPos + (veci2)lastSourceMotionVector;

            // if it's still correct, let it be
            if (!frame.inBounds(lastOffsetPos) || currentSearchColor != totalTexture.getValueUnsafe(lastOffsetPos))
            {
                cveci2 &blockMovementPos = lastMotionVectorArray.getValueUnsafe(pixelPos / lastMotionBlockSize);
                cveci2 &offsetPos = blockMovementPos + pixelPos;

                // check pixels left or right for the same pixel (did it move or did it disappear?)

                const auto &checkFunction = [&frame, &offsetPos, this, &currentSearchColor](cveci2 &relativePosition)
                {
                    cveci2 &checkPos = offsetPos + relativePosition;
                    return frame.inBounds(checkPos) && isRoughlyTheSameColor(totalTexture.getValueUnsafe(checkPos), currentSearchColor);
                };
                veci2 finalPos;
                if (findInCircles(2, checkFunction, finalPos))
                {
                    sourceMotionVectors.setValueUnsafe(pixelPos, compressedSourceMotionVector(blockMovementPos + finalPos));
                }
            }
        }
    }

    // apply source motion vectors
    addMotionVectors();

    // if (totalMovementVector != veci2())
    //{
    //     textureRGB texCopy = totalTexture;
    //     // move the whole texture
    //     auto movedRect = texCopy.getClientRect();
    //     movedRect.pos0 += totalMovementVector;
    //     fillTransformedBrushRectangle(rectangle2(texCopy.getClientRect()), vec2(totalMovementVector), texCopy, totalTexture);
    // }

    const colorRGB *const &totalTexEndPtr = (colorRGB *)totalTexture.end();
    colorRGB *diffPtr = (colorRGB *)diffTex.baseArray;
    colorRGB *framePtr = (colorRGB *)frame.baseArray;
    for (colorRGB *totalTexPtr = (colorRGB *)totalTexture.baseArray; totalTexPtr < totalTexEndPtr; framePtr++, diffPtr++, totalTexPtr++)
    {
        if (isRoughlyTheSameColor(*framePtr, *totalTexPtr))
        {
            *diffPtr = nothingChangedColor;
        }
        else
        {
            // we
            if (*framePtr == nothingChangedColor)
            {
                *diffPtr = colorRGB(0x79, 0x79, 0x80);
            }
            else
            {
                *diffPtr = *framePtr;
            }
            *totalTexPtr = *diffPtr;
        }
    }
    return diffTex;
}

void videoEncoder::serializeMotionVectors(nbtSerializer &serializer)
{
    sbyte *ptr = nullptr;
    int count;

    // declare the string outside so it doesn't go out of scope
    std::string compressedVectors;
    if (serializer.write)
    {
        compressedVectors = gzip::compress((const char *)sourceMotionVectors.baseArray, sourceMotionVectors.size.volume() * sizeof(compressedSourceMotionVector));
        count = compressedVectors.size();
        ptr = (sbyte *)&compressedVectors[0];
    }
    // serialize the motion vectors as one big char array
    serializer.serializeVariableArray<sbyte>(std::wstring(L"motion vectors"), ptr, count);
    if (!serializer.write)
    {
        std::string decomp = gzip::decompress((char *)ptr, count);
        std::copy(decomp.begin(), decomp.end(), (char *)sourceMotionVectors.baseArray);
    }
}

void videoEncoder::visualize(const texture &screen)
{

    // visualize the moved rects
    crectanglei2 &blocksRect = rectanglei2(sourceMotionVectors.size);
    for (cveci2 &blockPos : blocksRect)
    {
        crectanglei2 &rectToMove = rectanglei2(blockPos * gridStepSize, cveci2(gridStepSize));
        cveci2 &vec = sourceMotionVectors.getValueUnsafe(blockPos);
        if (vec != veci2())
        {
            fillRectangleBorders(screen, rectToMove, 1, brushes::red);
            constexpr int motionMult = 30;
            fillLine(screen, rectToMove.pos0, rectToMove.pos0 + vec * motionMult, brushes::green);
        }
    }
}

void videoEncoder::addMotionVectors() const
{
    // copy texture
    // const textureRGB oldTexture = totalTexture;
    // for (cveci2 &blockPos : sourceMotionVectors.getClientRect())
    //{
    //    veci2 movement = sourceMotionVectors.getValueUnsafe(blockPos);
    //    if(movement != veci2()){
    //        totalTexture.setValueUnsafe(blockPos + movement, oldTexture.getValue(blockPos));
    //    }
    //    //crectanglei2 &rectToMove = rectanglei2(blockPos * gridStepSize, cveci2(gridStepSize));
    //    //totalTexture.copyArray(rectToMove, rectToMove.pos0 + sourceMotionVectors.getValueUnsafe(blockPos), oldTexture);
    //    // moveArrayPart(totalTexture, rectToMove, sourceMotionVectors.getValueUnsafe(blockPos));
    //}
    // copy
    textureRGB oldTexture = totalTexture;
    for (cveci2 &pixelPos : sourceMotionVectors.getClientRect())
    {
        cveci2 &vector = sourceMotionVectors.getValueUnsafe(pixelPos);
        if (vector != compressedSourceMotionVector())
        {
            totalTexture.setValueUnsafe(pixelPos, oldTexture.getValueUnsafe(pixelPos + vector));
        }
    }
}

void videoEncoder::resizeGrid()
{
    sourceMotionVectors = array2d<compressedSourceMotionVector>(totalTexture.size);
}