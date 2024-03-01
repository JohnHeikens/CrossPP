#include "enchantmentID.h"
#include <vector>
#pragma once
const std::vector<enchantmentID> incompatibleEnchantments[]
{
	{enchantmentID::sharpnessEnchantment, enchantmentID::smiteEnchantment, enchantmentID::baneOfArthropodsEnchantment},
	{enchantmentID::fortuneEnchantment, enchantmentID::silkTouchEnchantment},
	{enchantmentID::protectionEnchantment, enchantmentID::blastProtectionEnchantment, enchantmentID::fireProtectionEnchantment, enchantmentID::projectileProtectionEnchantment},
	{enchantmentID::depthStriderEnchantment, enchantmentID::frostWalkerEnchantment},
	{enchantmentID::infinityEnchantment, enchantmentID::mendingEnchantment},
	{enchantmentID::multiShotEnchantment, enchantmentID::piercingEnchantment},
	{enchantmentID::loyaltyEnchantment, enchantmentID::riptideEnchantment},
	{enchantmentID::channelingEnchantment, enchantmentID::riptideEnchantment},
	{enchantmentID::silkTouchEnchantment, enchantmentID::lootingEnchantment},
	{enchantmentID::silkTouchEnchantment, enchantmentID::luckOfTheSeaEnchantment},
};


bool canCombine(const enchantmentID& enchantment0, const enchantmentID& enchantment1);