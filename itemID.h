#include "blockID.h"
#include "entityID.h"
#include "constants.h"
#include "harvestTier.h"
#include "toolTypeID.h"
#include "armorTier.h"
#include "armorType.h"

constexpr int normalTreeTypeCount = 6;
constexpr int netherTreeTypeCount = 2;

constexpr int musicDiscTypeCount = 13;
#pragma once
enum class itemID : int
{
	dye = (int)blockID::count,
	music_disc = dye + (int)colorID::count,
	stick = music_disc + musicDiscTypeCount,
	charcoal,
	clay_ball,
	leather,
	paper,
	book,
	enchanted_book,
	blaze_rod,
	blaze_powder,
	gunpowder,
	ghast_tear,
	magma_cream,
	flint,
	feather,
	string,
	arrow,
	brick,
	nether_brick,
	clock,
	fermented_spider_eye,
	glowstone_dust,
	heart_of_the_sea,
	honey_comb,
	ink_sac,
	nautilus_shell,
	nether_star,
	netherite_ingot,
	netherite_scrap,
	phantom_membrane,
	popped_chorus_fruit,
	prismarine_crystals,
	prismarine_shard,
	rabbit_hide,
	rabbit_foot,
	scute,
	shulker_shell,
	slime_ball,
	firework_star,
	firework_rocket,

	glass_bottle,
	honey_bottle,
	experience_bottle,
	dragon_breath,

	awkward_potion,
	mundane_potion,
	thick_potion,

	potion,
	splash_potion,
	lingering_potion,

	bow,
	egg,
	fishing_rod,
	snowball,
	ender_pearl,
	ender_eye,

	end_crystal,

	wheat_seeds,
	carrot,
	potato,
	beetroot,

	melon_seeds,
	pumpkin_seeds,
	glistering_melon_slice,
	melon_slice,
	poisonous_potato,
	sweet_berries,
	chorus_fruit,
	dried_kelp,
	apple,
	beetroot_soup,
	baked_potato,
	rotten_flesh,
	golden_carrot,
	porkchop,
	cooked_porkchop,
	beef,
	cooked_beef,
	mutton,
	cooked_mutton,
	chicken,
	cooked_chicken,
	rabbit,
	cooked_rabbit,
	cod,
	cooked_cod,
	salmon,
	cooked_salmon,
	tropical_fish,
	pufferfish,
	spider_eye,
	bread,
	golden_apple,
	enchanted_golden_apple,
	mushroom_stew,
	rabbit_stew,

	bowl,
	bucket,
	water_bucket,
	lava_bucket,
	milk_bucket,
	flint_and_steel,

	bone,
	bone_meal,

	//ore drops respectively
	coal,
	iron_ingot,
	emerald,
	lapis_lazuli,
	redstone,
	gold_ingot,
	diamond,
	quartz,
	goldnugget,
	ironnugget,

	spawn_egg,

	minecart = spawn_egg + mobTypeCount,
	chest_minecart,
	furnace_minecart,
	hopper_minecart,
	tnt_minecart,
	command_block_minecart,

	wood_boat,
	material_tool = wood_boat + normalTreeTypeCount,//x: tool type, y: tool tier
	shears = material_tool + toolTierCount * materialToolTypeCount,
	elytra,
	turtle_helmet,
	material_armor,//x: armor type, y: armor tier
	count = material_armor + (armorTierCount - 1) * armorTypeCount
};

constexpr int mineCartItemTypeCount = (int)itemID::command_block_minecart + 1 - (int)itemID::minecart;
