#include "constants.h"
#include "world.h"
#include "include/filesystem/textfile.h"
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
			//TODO: message box cross-platform
			//int msgboxID = MessageBox(
			//	NULL,
			//	(LPCWSTR)message.c_str(),
			//	(LPCWSTR)gameName.c_str(),
			//	MB_ICONWARNING | MB_OK
			//);
			throw std::runtime_error(messageToString);
		}
	}
}
[[noreturn]] void handleCrash(const std::wstring &message)
{
	//create crash log
	std::string strMessage = WStringToString(message);
	writealltext(stdPath(L"crashlog.txt"), strMessage);
	throw std::runtime_error("crash log saved in crashlog.txt");
}
fp getPowerStrength(const powerLevel &level)
{
	constexpr fp multiplier = powerStrengthStepCount / maxPowerLevel;

	return pow(2, (level * multiplier) - powerStrengthStepCount);
}