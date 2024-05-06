#include "breedableMob.h"
#include "nbtSerializer.h"
void breedableMob::tick()
{
	age++;
}

void breedableMob::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"age"), age);
}