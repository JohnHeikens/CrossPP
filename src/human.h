#pragma once
#include "humanoid.h"
#include "GamemodeID.h"

#include "humanSlotContainerUI.h"
#include "anvilSlotContainer.h"
#include "smithingTableSlotContainer.h"
#include "dispenserSlotContainer.h"
#include "enchantingTableSlotContainer.h"
#include "brewingStandSlotContainer.h"
#include "chestSlotContainer.h"
#include "furnaceSlotContainer.h"
#include "craftingTableSlotContainer.h"
#include "soundhandler2d.h"

//extern humanSlotContainerUI* humanSlots;
//extern craftingTableSlotContainer* craftingTableSlots;
//extern furnaceSlotContainer* furnaceSlots;
//extern chestSlotContainer* chestSlots;
//extern brewingStandSlotContainer* brewingStandSlots;
//extern enchantingTableSlotContainer* enchantmentSlots;
//extern dispenserSlotContainer* dispenserSlots;
//extern smithingTableSlotContainer* smithingTableSlots;
//extern anvilSlotContainer* anvilSlots;
//extern std::shared_ptr<audio2d> currentWindSound;
struct playerSocket;
struct gameControl;
struct human :public humanoid, public INamable
{
	//containers
	humanSlotContainerUI* humanSlots = nullptr;
	craftingTableSlotContainer* craftingTableSlots = nullptr;
	furnaceSlotContainer* furnaceSlots = nullptr;
	chestSlotContainer* chestSlots = nullptr;
	brewingStandSlotContainer* brewingStandSlots = nullptr;
	enchantingTableSlotContainer* enchantmentSlots = nullptr;
	dispenserSlotContainer* dispenserSlots = nullptr;
	smithingTableSlotContainer* smithingTableSlots = nullptr;
	anvilSlotContainer* anvilSlots = nullptr;

	gameControl& screen;
	rectangularSlotContainer* hotbarSlots = nullptr;
	rectangularSlotContainer* leftHandSlot = nullptr;
	rectangularSlotContainer* inventorySlots = nullptr;

	gameModeID currentGameMode = gameModeID::survival;

	int experience = 0;
	int score = 0;

	//stomach
	int foodlevel = maxhumanfoodlevel;
	fp foodsaturationlevel = 5;//saturationlevel cannot exceed foodlevel
	fp foodExhaustionlevel = 0;
	byte foodticktimer = 0;

	int foodAnimationTime = 0;

	tickableBlockContainer* selectedContainerContainer = nullptr;
	veci2 selectedContainerPosition = veci2();

	bool wantsToKeepUsing = false;
	bool wantsToStartUsing = false;
	bool wantsToStopUsing = false;

	void addExperience(cint& amount);
	void calculateFood();
	void addExhaustion(cfp& exhaustion);
	using humanoid::calculateHitBox;
	virtual rectangle2 calculateHitBox(cvec2& position) const override;

	//hands
	int rightHandSlotIndex = 0;
	bool seenCredits = false;

	//drop the itemstack as if the player dropped it
	void drop(itemStack& s);
	vec2 getDropPosition();

	void pickUpFloatingSlots();
	bool canSleep() const;
	human(dimension* dimensionIn, cvec2& position, gameControl& screen, const std::wstring& name);
	virtual void tick() override;
	virtual void applyStatusEffect(const statusEffect& effect) override;
	virtual void onDeath() override;
	virtual ~human() override;
	virtual void serializeValue(nbtSerializer& s) override;
	bool serialize(cbool& write);
	virtual bool addDamageSource(cfp& damage, std::shared_ptr<damageSource> source) override;
	virtual bool compareSelector(const human& sender, const std::wstring& selectorString) const override;
	virtual bool addStack(itemStack& stack) override;
	void addStackOrDrop(itemStack& stack);
	virtual bool substractStack(itemStack& stack) override;
	virtual std::vector<vec3> getFrictions() const override;
	virtual void render(const gameRenderData& targetData) const override;
	void setGameMode(const gameModeID& newGameMode);
	void closeBlockGUI();


	void onItemRightClick(itemStack& stackIn);
	void onItemRightClickReleased(itemStack& stackIn);
	void linkUpInventories() const;

	virtual void respawn() override;
};