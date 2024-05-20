#include "math/graphics/texture.h"
#pragma once
//typedef int fastColor;
//typedef array2d<colorRGB> textureRGB;
//typedef array2d<fastColor> fastTexture;
struct nbtSerializer;
constexpr byte motionCheckStepCount = 3;

typedef vect2<short> compressedSourceMotionVector;
enum class videoEncoderType{
    decoder,
    encoder
};
//template <videoEncoderType t>
struct videoEncoder
{
    bool shouldCompress = false;
    // when the screen is moving to the right, the pixels from the left will be copied.
    // so the source motion vectors will be -x while the screen goes to +1
    array2d<compressedSourceMotionVector> sourceMotionVectors = array2d<compressedSourceMotionVector>();

    array2d<veci2> motionBlockVectors[motionCheckStepCount];
    std::vector<byte> decompressedScreen = std::vector<byte>();

    // the screen players will see
    texture totalTexture = texture(veci2());

    //CAUTION! this texture points to decompressedscreen.begin()!
    //(a trick to have less copying)
    texture diffTex = texture(veci2());

    // the 'perfect' screen. this is what we aim for
    // textureRGB totalCorrectTexture = textureRGB(veci2());
    void addFrameDiff(nbtSerializer &serializer);
    void resizeGrid();
    void addMotionVectors() const;
    void serializeScreen(nbtSerializer& serializer);
    void addFrame(const texture &frame, nbtSerializer &serializer);
    void serializeMotionVectors(nbtSerializer &serializer);
    void visualize(const texture &screen);
    ~videoEncoder();
};