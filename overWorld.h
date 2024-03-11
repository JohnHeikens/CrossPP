#pragma once
#include "dimension.h"
#include "include/math/LayerNoiseSimplex.h"
struct overWorld :dimension
{
	overWorld();

	layerNoiseSimplex* caveNoise = nullptr;

	layerNoiseSimplex* biomeElevationNoise = nullptr;
	layerNoiseSimplex* biomeTemperatureNoise = nullptr;
	layerNoiseSimplex* biomeHumidityNoise = nullptr;

	layerNoiseSimplex* cloudThicknessNoise = nullptr;

	layerNoiseSimplex* windSpeedNoise = nullptr;
	layerNoiseSimplex* windDirectionNoise[2]{};

	virtual generationData* generateTerrain(chunk* generateIn) override;
	virtual void generateStructures(chunk* generateIn) override;
	virtual void renderSky(crectangle2& blockRect, crectangle2& drawRect, const gameRenderData& targetData) const override;
	virtual biomeID getBiome(cvec2& position) const override;
	virtual ~overWorld() override;

	virtual void initialize() override;
	static biomeID getBiome(cfp& elevation, cfp& temperature, cfp& humidity) noexcept;


	virtual vec2 getWindSpeed(cvec2& position) override;
};