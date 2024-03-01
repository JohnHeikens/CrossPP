#pragma once

static constexpr int hotbarSlotKeys[StandardInventoryColumnCount]
{
	0x31,
	0x32,
	0x33,
	0x34,
	0x35,
	0x36,
	0x37,
	'C',
	'F',
};

enum class keyID :int
{
	hotbarSlot0 = hotbarSlotKeys[0],
	hotbarSlot1 = hotbarSlotKeys[1],
	hotbarSlot2 = hotbarSlotKeys[2],
	hotbarSlot3 = hotbarSlotKeys[3],
	hotbarSlot4 = hotbarSlotKeys[4],
	hotbarSlot5 = hotbarSlotKeys[5],
	hotbarSlot6 = hotbarSlotKeys[6],
	hotbarSlot7 = hotbarSlotKeys[7],
	hotbarSlot8 = hotbarSlotKeys[8],
	left = 'A',
	right = 'D',
	up = 'W',//'W',
	down = 'S',//'S',
	inventory = 'E',
	drop = 'M',//not q, because you will accidentally drop the worst items, like swords
	camera = 'F',
	hit = VK_LBUTTON,
	use = VK_RBUTTON,
	jump = VK_SPACE,
	sprint = VK_CONTROL,
	sneak = VK_SHIFT,
	exit = sneak,
	commandLine = VK_OEM_2,//backslash or question mark
	text = 'T',
	debug = VK_F3,
	headUpDisplay = VK_F1,
	escape = VK_ESCAPE,
	renderHitboxes = 'B',
	count
};