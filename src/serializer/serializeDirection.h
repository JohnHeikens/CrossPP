#include "math/direction.h"
#include "nbtSerializer.h"
inline bool serializeNBTValue(nbtSerializer& s, const std::wstring &memberName, directionID &direction)
{
    return s.serializeValue(memberName, (sbyte &)direction);
}