#include "math/graphics/texture.h"
#pragma once
typedef array2d<colorRGB> textureRGB;
struct nbtSerializer;
struct videoEncoder
{
    //when the screen is moving to the right, the pixels from the left will be copied.
    //so the source motion vectors will be -x while the screen goes to +1
    array2d<vect3<sbyte>> sourceMotionVectors = array2d<vect3<sbyte>>();
    textureRGB totalTexture = textureRGB(veci2());
    void addFrameDiff(const textureRGB &diffTex, nbtSerializer& serializer);
    void resizeGrid();
    void addMotionVectors() const;
    textureRGB addFrame(const textureRGB &frame);
    void serializeMotionVectors(nbtSerializer &serializer);
    void visualize(const texture& screen);
};