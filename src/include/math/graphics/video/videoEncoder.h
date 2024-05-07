#include "math/graphics/texture.h"
#pragma once
typedef array2d<colorRGB> textureRGB;
struct nbtSerializer;
typedef vect3<sbyte> compressedSourceMotionVector;
struct videoEncoder
{
    //when the screen is moving to the right, the pixels from the left will be copied.
    //so the source motion vectors will be -x while the screen goes to +1
    array2d<compressedSourceMotionVector> sourceMotionVectors = array2d<compressedSourceMotionVector>();
    //the screen players will see
    textureRGB totalTexture = textureRGB(veci2());
    //the 'perfect' screen. this is what we aim for
    //textureRGB totalCorrectTexture = textureRGB(veci2());
    void addFrameDiff(const textureRGB &diffTex, nbtSerializer& serializer);
    void resizeGrid();
    void addMotionVectors() const;
    textureRGB addFrame(const textureRGB &frame);
    void serializeMotionVectors(nbtSerializer &serializer);
    void visualize(const texture& screen);
};