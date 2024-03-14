#pragma once
#include "IItemComparable.h"
#include "interface/idestructable.h"
#include "array/fastlist.h"
//represents a list of items that hold this tag
struct tag :IItemComparable, IDestructable
{
	std::wstring name = std::wstring(L"");
	fastList<IItemComparable*>* taggedComparables = nullptr;
	tag(std::wstring name) :name(name), taggedComparables(new fastList<IItemComparable*>()) {}
	bool hasTaggedItems();
	//copy tagged items to this tag
	virtual bool compare(const itemID& itemToCompare) const override;
	virtual ~tag() override;
};

tag* readTag(const std::wstring& tagName, const std::wstring& tagDirectory);
size_t getTagListIndexByName(std::wstring name);
extern fastList<tag*> tagList;