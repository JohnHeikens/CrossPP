#pragma once
#include "globalFunctions.h"
#include "math/vector/vectn.h"
enum mouseEventType{
    down,
    move,
    up
};
struct mouseHistoryEvent {
    mb button;
    mouseEventType type;//false = released
    veci2 position;
    mouseHistoryEvent(cveci2& position, cmb& button = (mb)-1, const mouseEventType& type = mouseEventType::down) : position(position), button(button), type(type) {}
};