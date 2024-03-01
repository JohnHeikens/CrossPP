#pragma once
#include "array/wstring.h"
#include "fileio.h"
#include <filesystem>
#include <fstream>
#include <iostream>

//reads an entire file into a string.
//source
//https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
inline std::string readalltext(std::wstring path)
{

	std::ifstream t(path, getOpenMode(false));
	t.seekg(0, std::ios::end);
	size_t size = (size_t)t.tellg();
	std::string buffer(size, '#');
	t.seekg(0);
	t.read(&buffer[0], (std::streamsize)size);
	return buffer;
}
inline void writealltext(std::wstring path, std::string text)
{
	std::ofstream s(path, getOpenMode(true));
	s.write(text.c_str(), (std::streamsize)text.size());
	s.close();
}
//https://www.codespeedy.com/fetch-a-random-line-from-a-text-file-in-cpp/
inline stringContainer readAllLines(std::wstring path)
{
	std::string line;
	stringContainer lines;
	//input file stream
	std::ifstream file(path);

	//store the lines in the string vector
	while (std::getline(file, line))
	{
		lines.push_back(line);
	}
	return lines;
}



