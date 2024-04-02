#pragma once
#include "interface/idestructable.h"
#include <string>
#include "filesystem/filemanager.h"
struct audioCollection :IDestructable
{
	void addAudioFileName(const stdPath& path);
	virtual void addAudioFile(const stdPath& path);

};