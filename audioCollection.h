#pragma once
#include "include/interface/idestructable.h"
#include <string>
struct audioCollection :IDestructable
{
	void addAudioFileName(const std::wstring& path);
	virtual void addAudioFile(const std::wstring& path);

};