#pragma once
#include "include/math/neuralnetwork.h"
#include "include/math/neuralnetwork/aiinputdata.h"
#include "EntityAI.h"
#include "include/math/fastlayer.h"

constexpr size_t aiVisionSize = 0x8;

//one pixel = 2*2 blocks
constexpr fp aiVisibleRange = aiVisionSize;

constexpr fp learningSpeedMomentumMultiplier = 1.0 - (1.0 / ticksPerHour);

//learns in an hour
constexpr fp learningRate = 1.0 / ticksPerHour;

struct selfLearningAI : entityAI
{
	//distance to block intersection / reach
	aiInputData blockDistanceInput = aiInputData(1);
	//distance to entity intersection / reach 
	aiInputData entityDistanceInput = aiInputData(1);

	//if it is selecting an entity
	aiInputData entitySelectingInput = aiInputData(1);

	//how much it's in pain
	aiInputData painInput = aiInputData(1);

	//how much of it health is left
	aiInputData healthInput = aiInputData(1);

	//attack cooldown
	aiInputData attackCooldownInput = aiInputData(1);

	//sound loudness, left + right
	aiInputData soundLoudnessInput = aiInputData(2);

	//wind speed (x, y)
	aiInputData windSpeedInput = aiInputData(2);

	//if it collided(x, y)
	aiInputData axisCollidedInput = aiInputData(2);

	//the speed it has(x, y)
	aiInputData speedInput = aiInputData(2);

	//rgb values of surrounding blocks: render game to mini screen (0x8 by 0x8)
	aiInputData visionInput = aiInputData(aiVisionSize * aiVisionSize * 3);

	//output:

	//if it wants to go in 4 directions
	aiOutputData wantsToGoOutput = aiOutputData(4);

	//if it wants to sneak
	aiOutputData wantsToSneakOutput = aiOutputData(1);

	//if it wants to sprint
	aiOutputData wantsToSprintOutput = aiOutputData(1);

	//if it wants to sneak
	aiOutputData wantsToJumpOutput = aiOutputData(1);

	//non-passive mob: if it wants to hit
	aiOutputData wantsToHitOutput = aiOutputData(1);

	texture wolfScreen = texture(cvect2<size_t>(aiVisionSize));

	fastLayer* correctAnswers = nullptr;

	void evaluate() const;

	void execute() override;

	void serializeWithOutput(cbool& isCorrectAnswer) const;

	void errorWithCorrectInput();

	//virtual void serializeValue(nbtSerializer& s) override;

	neuralNetwork* brains = nullptr;
	~selfLearningAI();
	selfLearningAI(entity* const& connectedEntity);

	fp lastError = 0;
};