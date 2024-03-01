#include "potionData.h"
#include "textureList.h"
#include "snowLayerData.h"
#include "idAnalysis.h"
#include "include/application/control/control.h"
#include "itemStack.h"
#include "itemData.h"
#include "durabilityData.h"
#include "world.h"
#include "statusEffectData.h"
#include "idConverter.h"
#include "renderBlockContainer.h"
itemStack::itemStack(const itemStack& other)
{
	enchantments = std::vector<enchantment*>();
	for (int i = 0; i < other.enchantments.size(); i++)
	{
		enchantments.push_back((enchantment*)new enchantment(other.enchantments[i]->clone<enchantment>()));
	}

	data = other.count ? createItemTag(other.stackItemID) : nullptr;
	if (data)
	{
		other.data->clone(*data);
	}
	stackItemID = other.stackItemID;
	count = other.count;
}

void itemStack::add(cint& count)
{
	this->count += count;
	if (this->count == 0)
	{
		clearData();
	}
	else if (isDebugging && this->count < 0)
	{
		handleError(std::wstring(L"negative stack"));
	}
}

void itemStack::swap(itemStack& with)
{
	std::swap(enchantments, with.enchantments);
	std::swap(count, with.count);
	std::swap(stackItemID, with.stackItemID);
	std::swap(data, with.data);
}

bool itemStack::compare(const itemStack& other) const
{
	if (other.stackItemID != stackItemID)
	{
		return false;
	}
	if (other.data == nullptr)
	{
		if (data != nullptr)
		{
			return false;
		}
	}
	else
	{
		if (data == nullptr || !data->compare(*other.data))
		{
			return false;
		}
	}
	if (other.enchantments.size() != enchantments.size())
	{
		return false;
	}
	for (int i = 0; i < other.enchantments.size(); i++)
	{
		if (!other.enchantments[i]->compare(*enchantments[i]))
		{
			return false;
		}
	}
	return true;
}

bool itemStack::addStack(itemStack& s, int amount)
{
	if (s.count < amount)
	{
		//the amount of items to add = the amount this slot has
		amount = s.count;
	}
	if (amount)
	{
		if (count)
		{
			//there are already items in this slot
			//check if they are the same items
			if ((count < itemList[stackItemID]->maxStackSize) && compare(s))
			{
				int room = itemList[stackItemID]->maxStackSize - count;
				if (amount > room)
				{
					//cant deposit all items
					//maximize the stack
					count = itemList[stackItemID]->maxStackSize;
					//the amount of the items to add = the room there is
					s.count -= room;
					return false;//not all items have been added
				}
				else
				{
					//add all items
					add(amount);
					s.add(-amount);
					return true;
				}
			}
			else
			{
				return false;//can't add, as they're not the same items
			}
		}
		else
		{
			*this = s;
			this->count = amount;
			s.add(-amount);
			return true;
		}
	}
	return true;//this is an empty slot, so what does it matter?
}

bool itemStack::addStack(itemStack& s)
{
	return addStack(s, s.count);
}

void itemStack::clearData()
{
	if (data)
	{
		delete data;
	}
	for (enchantment* e : enchantments)
	{
		delete e;
	}
	stackItemID = (itemID)0;
	enchantments = std::vector<enchantment*>();
	data = nullptr;
	count = 0;
}

itemStack::~itemStack()
{
	clearData();
}

void itemStack::render(crectangle2& rect, const texture& renderTarget) const
{
	if (count)
	{
		const resolutionTexture& textureToUse = replaceIfMissing(itemList[stackItemID]->tex);
		cmat3x3& matrix = mat3x3::fromRectToRect(crectangle2(textureToUse.getClientRect()), rect);
		renderSingleItem(matrix, renderTarget);
		if (count > 1)
		{
			//draw text
			std::wstring s = std::to_wstring(count);
			cveci2 size = cveci2((int)(s.size() * defaultTheme()->font->fontSize), (int)defaultTheme()->font->fontSize);
			cvec2 topRight = rect.pos1();
			defaultTheme()->font->DrawString(s, rectangle2(topRight.x() - size.x(), rect.pos0.y(), size.x(), size.y()), renderTarget);
		}

		if (hasDurability(stackItemID))
		{
			durabilityData* toDurabilityData = (durabilityData*)data;

			if (toDurabilityData->durability < 1)
			{
				cfp& margin = 0.1;
				crectangle2& durabilityBarRect = getAbsoluteRect(rect, crectangle2(margin, margin, 1 - (margin * 2), 0.15));

				renderTarget.fillRectangle(durabilityBarRect, brushes::black);

				crectangle2& durabilityRect = getAbsoluteRect(durabilityBarRect, crectangle2(0, 0.5, toDurabilityData->durability, 0.5));

				ccolor& durabilityColor = color::lerpcolor(colorPalette::red, colorPalette::green, toDurabilityData->durability);
				renderTarget.fillRectangle(durabilityRect, solidColorBrush(durabilityColor));
			}
		}
	}
}

#pragma optimize( "", off )
void itemStack::renderSingleItem(cmat3x3& transform, const texture& renderTarget) const
{
	if (count)
	{
		const resolutionTexture& textureToUse = replaceIfMissing(itemList[stackItemID]->tex);

		crectangle2 textureRect = crectangle2(textureToUse.getClientRect());

		if (enchantments.size() || (stackItemID == itemID::enchanted_golden_apple))
		{
			const auto& repeater = repeatingBrush<resolutionTexture>(*enchantedItemTexture);
			const auto& transformer = transformBrush<repeatingBrush<resolutionTexture>>(mat3x3::translate(cvec2(0, (currentWorld->ticksSinceStart / ((fp)ticksPerRealLifeSecond * 5)) * enchantedItemTexture->defaultSize.y())), repeater);
			//keep this apart, so the reference is not destroyed
			const auto& transparencyBrush = solidColorBrush(color((colorChannel)(color::maxValue * 0.4), 0));
			const auto& transparencyMask = alphaMask<solidColorBrush, decltype(transformer)>(transparencyBrush, transformer);

			const auto& mixer = colorMixer <decltype(transparencyMask), resolutionTexture>(transparencyMask, textureToUse);

			const auto& screenMask = alphaMask<resolutionTexture, decltype(mixer)>(textureToUse, mixer);

			fillTransparentRectangle(textureRect, transform, screenMask, renderTarget);
		}
		else
		{
			if (isBlockItem(stackItemID) && !isDoubleBlock((blockID)stackItemID)) {
				//transform from block container to texture
				cmat3x3& matrix = mat3x3::cross(transform, mat3x3::fromRectToRect(crectangle2(0, 0, 1, 1), textureRect));
				const blockID& itemBlock = (blockID)stackItemID;
				cveci2& blockPosition = cveci2();
				const renderData& targetData = renderData(matrix, renderTarget);
				renderBlockContainer container = renderBlockContainer(cveci2(1));
				container.setBlockID(cveci2(), (blockID)stackItemID);
				blockList[itemBlock]->render(targetData, container.getBlockData(blockPosition), &container, blockPosition);
			}
			else {
				fillTransparentRectangle(textureRect, transform, textureToUse, renderTarget);
			}
		}
		if (isPotion(stackItemID))
		{
			color potionColor;
			if (canAddEffects(stackItemID) && ((potionData*)data)->effectsToAdd.size())
			{
				potionData* toPotionData = (potionData*)data;
				//average out
				colorf totalColor = colorf();
				for (int i = 0; i < toPotionData->effectsToAdd.size(); i++)
				{
					totalColor += colorf(statusEffectDataList[(int)toPotionData->effectsToAdd[i].identifier]->particleColor);
				}
				totalColor /= toPotionData->effectsToAdd.size();
				potionColor = color(totalColor);
			}
			else
			{
				potionColor = waterColor;
			}
			const solidColorBrush& potionColorBrush = solidColorBrush(potionColor);
			mat3x3 mipmapTransform = transform;
			rectangle2 rect = crectangle2(blockTextureRect);
			const auto& mult = colorMultiplier<texture, solidColorBrush>(potionOverlayTexture->mipmap(mipmapTransform, rect), potionColorBrush);
			fillTransparentRectangle(rect, mipmapTransform, mult, renderTarget);
		}
	}
}
#pragma optimize( "", on )

int itemStack::getEnchantmentLevel(const enchantmentID& identifier) const
{
	for (enchantment* e : enchantments)
	{
		if (e->identifier == identifier)
		{
			return e->power;
		}
	}
	return 0;
}

void itemStack::drawToolTips(cveci2& position, const texture& renderTarget) const
{
	const std::wstring& nameText = replace(itemList[stackItemID]->name, std::wstring(L"_"), std::wstring(L" "));
	std::wstring stringToDraw = nameText;
	std::wstring enchantmentsString = std::wstring(L"");
	for (int i = 0; i < enchantments.size(); i++)
	{
		if (i > 0)
		{
			enchantmentsString += std::wstring(L"\n");
		}
		else
		{
			stringToDraw += std::wstring(L"\n");
		}
		enchantment* e = enchantments[i];
		enchantmentsString += e->toWString();
	}
	stringToDraw += enchantmentsString;

	cvec2 maximumSize = cvec2(0x200, 0x300);
	cvec2 rectangleSize = defaultTheme()->font->measureStringSize(maximumSize, stringToDraw);
	crectangle2 stringRect = crectangle2(position.x(), position.y() - rectangleSize.y(), rectangleSize.x(), rectangleSize.y());
	renderTarget.fillRectangle(stringRect.expanded(defaultTheme()->borderSize), brushes::black);
	const font nameFont = font(defaultTheme()->font->family, defaultTheme()->font->fontSize, &brushes::cyan);
	nameFont.DrawString(nameText, stringRect, renderTarget);
	crectangle2 enchantmentsStringrect = crectangle2(stringRect.x(), stringRect.y(), stringRect.w(), stringRect.h() - defaultTheme()->font->fontSize);
	defaultTheme()->font->DrawString(enchantmentsString, enchantmentsStringrect, renderTarget);
}

void itemStack::serializeValue(nbtSerializer& s)
{
	if (s.write)
	{
		s.serializeValue(std::wstring(L"item id"), (int&)stackItemID);
	}
	else
	{
		if (idConverter::serializeID(s, std::wstring(L"item id"), stackItemID, s.converter ? &s.converter->itemIDConverter : nullptr))
		{

		}
	}
	if ((int)stackItemID) //only store additional data of filled slots
	{
		s.serializeValue(std::wstring(L"amount"), count);
		if (s.push< nbtDataTag::tagList>(std::wstring(L"enchantments")))
		{
			if (s.write)
			{
				for (enchantment* e : enchantments)
				{
					if (s.push<nbtDataTag::tagCompound>())
					{
						e->serializeValue(s);
						s.pop();
					}
				}
			}
			else
			{
				std::vector<nbtData*> serializedEnchantmentData = s.getChildren();
				enchantments = std::vector < enchantment*>(serializedEnchantmentData.size());
				for (int i = 0; i < enchantments.size(); i++)
				{
					if (s.push(serializedEnchantmentData[i]))
					{
						enchantments[i] = new enchantment();
						enchantments[i]->serializeValue(s);
						s.pop();
					}
				}
			}
			s.pop();
		}
		if (s.write)
		{
			if (data)
			{
				data->serialize(s, std::wstring(L"data"));
			}
		}
		else
		{
			data = createItemTag(stackItemID);
			if (data)
			{
				data->serialize(s, std::wstring(L"data"));
			}
		}
	}
}