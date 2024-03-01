#include "loveModeAI.h"
bool loveModeAI::shouldExecute()
{
	mob* connectedMob = (mob*)connectedEntity;
	if (connectedMob)
	{

	}

	return false;
}

void loveModeAI::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"partner found"), partnerFound);
}