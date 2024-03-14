#pragma once
struct keyHistoryEvent {
	vk key;
	bool down;//false = released
	keyHistoryEvent(cvk key = 0, cbool& down = false) : key(key), down(down) {}
};