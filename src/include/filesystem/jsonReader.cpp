#include "jsonReader.h"
#include "array/wstring.h"

jsonContainer readJson(std::wstring content)
{
	jsonContainer container = jsonContainer();
	content = replace(content, std::wstring(L"\\u0026"), std::wstring(L"&"));
	//{ = data with name
	//[ = data without name
	csize_t& indexQuote = find(content, 0, std::wstring(L"\""), std::wstring(L""));
	csize_t& indexCurlyBracket = find(content, 0, std::wstring(L"{"), std::wstring(L""));
	csize_t& indexBracket = find(content, 0, std::wstring(L"["), std::wstring(L""));
	csize_t& nearestIndex = FindNearest(std::vector<size_t>({ indexQuote,indexCurlyBracket,indexBracket }));
	if (nearestIndex == -1)
	{
		container.value = trim_copy(content);
	}
	if (nearestIndex == 0)
	{
		//" first
						//read name
		csize_t& startPos = indexQuote + 1;
		csize_t& endPos = find(content, startPos, std::wstring(L"\""), std::wstring(L""));
		container.value = content.substr(startPos, endPos - startPos);

		csize_t& indexColon = find(content, endPos + 1, std::wstring(L":"), std::wstring(L"{}[]()\"\""));
		if (indexCurlyBracket == -1 && indexBracket == -1 && indexColon != -1)
		{
			jsonContainer movingBlocks = jsonContainer();
			size_t indexQuote2 = find(content, endPos + 1, std::wstring(L"\""), std::wstring(L"{}[]"));
			if (indexQuote2 == -1)
			{
				movingBlocks.value = trim_copy(content.substr(indexColon + 1));
			}
			else
			{
				indexQuote2 += 1;
				csize_t& endIndexQuote2 = find(content, indexQuote2, std::wstring(L"\""), std::wstring(L""));
				movingBlocks.value = content.substr(indexQuote2, endIndexQuote2 - indexQuote2);
			}
			container.children.push_back(movingBlocks);
			return container;
		}
	}
	csize_t& nearestBracketIndex = FindNearest(std::vector<size_t>({ indexCurlyBracket,indexBracket }));
	if (nearestBracketIndex == 0) {
		//{ first
		csize_t& endIndexCurlyBracket = find(content, indexCurlyBracket + 1, std::wstring(L"}"), std::wstring(L"[]\"\"{}"));
		//separate by commas
		wstringContainer arrayContainer = split_string(content.substr(indexCurlyBracket + 1, endIndexCurlyBracket - indexCurlyBracket - 1), std::wstring(L","), std::wstring(L"[]\"\"{}"));
		for (std::wstring str : arrayContainer)
		{
			container.children.push_back(readJson(str));
		}
	}
	else if (nearestBracketIndex == 1)
	{//[ first
		csize_t& endIndexBracket = find(content, indexBracket + 1, std::wstring(L"]"), std::wstring(L"{}\"\"[]"));
		//separate by commas
		wstringContainer arrayContainer = split_string(content.substr(indexBracket + 1, endIndexBracket - indexBracket - 1), std::wstring(L","), std::wstring(L"[](){}"));
		for (std::wstring str : arrayContainer)
		{
			container.children.push_back(readJson(str));
		}
	}
	return container;
}

int jsonContainer::getChildIndex(const std::wstring& value) const
{
	for (int i = 0; i < children.size(); i++)
	{
		if (children[i].value == value)
		{
			return i;
		}
	}
	return -1;
}

jsonContainer jsonContainer::getChild(const std::wstring& value) const
{
	cint index = getChildIndex(value);
	if (index == -1)
	{
		throw "no child found with this value";
	}
	return children[index];
}

jsonContainer jsonContainer::operator[](const std::wstring& name) const
{
	return getChild(name);
}