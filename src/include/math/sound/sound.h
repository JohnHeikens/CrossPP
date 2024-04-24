#pragma once
#include "globalFunctions.h"

constexpr int octaveNoteCount = 12;


//returns 1 if the relative note is 0, 2 if it is 12 and so on
fp getNotePitch(cfp& relativeNote);