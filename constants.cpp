#include "constants.h"
#include "world.h"
void handleError(const std::wstring& message)
{
	const std::string& messageToString = WStringToString(message);
	if constexpr (isDebugging)
	{
		throw std::runtime_error(messageToString);
	}
	else
	{
		if (currentWorld)
		{
			currentWorld->currentChat.addLine(message);
		}
		else
		{
			int msgboxID = MessageBox(
				NULL,
				(LPCWSTR)message.c_str(),
				(LPCWSTR)L"Medieval Survival",
				MB_ICONWARNING | MB_OK
			);
			throw std::runtime_error(messageToString);
		}
	}
}
cfp getPowerStrength(const powerLevel& level)
{
	constexpr fp multiplier = powerStrengthStepCount / maxPowerLevel;

	return pow(2, (level * multiplier) - powerStrengthStepCount);
}