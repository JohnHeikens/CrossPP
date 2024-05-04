#include "math/graphics/texture.h"
#pragma once
typedef array2d<colorRGB> textureRGB;
struct nbtSerializer;
struct videoEncoder
{
    array2d<vect2<sbyte>> motionVectors = array2d<vect2<sbyte>>();
    textureRGB totalTexture = textureRGB(veci2());
    void addFrameDiff(const textureRGB &diffTex, nbtSerializer& serializer);
    void resizeGrid();
    void addMotionVectors() const;
    textureRGB addFrame(const textureRGB &frame);
    void serializeMotionVectors(nbtSerializer &serializer);
    void visualize(const texture& screen);
};