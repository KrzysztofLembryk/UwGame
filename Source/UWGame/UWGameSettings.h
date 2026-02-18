#pragma once

#include "Engine/DeveloperSettings.h"
#include "UWGameSettings.generated.h"

UCLASS(config=Game, DefaultConfig)
class UUWGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UUWGameSettings(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(Config, EditAnywhere)
	float NeighbourRadius = 300.f;

	UPROPERTY(Config, EditAnywhere)
	float NeighbourWolfRadius = 500.f;
	
	UPROPERTY(Config, EditAnywhere)
	float SeparationWeight = 1.3f;

	UPROPERTY(Config, EditAnywhere)
	float AlignmentWeight = 1.2f;

	UPROPERTY(Config, EditAnywhere)
	float CohesionWeight =  1.2f;

	UPROPERTY(Config, EditAnywhere)
	float WolfWeight = 2.f;

	UPROPERTY(Config, EditAnywhere)
	float MaxSpeed = 400.f;

	UPROPERTY(Config, EditAnywhere)
	float MaxForce = 100.f;

	UPROPERTY(Config, EditAnywhere)
	bool bDrawDebugs = true;

	UPROPERTY(Config, EditAnywhere)
	int32 SheepPointsMin = 1;

	UPROPERTY(Config, EditAnywhere)
	int32 SheepPointsMax = 15;

	UPROPERTY(Config, EditAnywhere)
	float DamageMultiplier = 1.1f;

	UPROPERTY(Config, EditAnywhere)
	float SpeedMultiplier = 1.1f;

	UPROPERTY(Config, EditAnywhere)
	float CactusRetaliationMultiplier = 0.8f;

	UPROPERTY(Config, EditAnywhere)
	float AdditionalTimeMultiplier = 1.25f;

	UPROPERTY(Config, EditAnywhere)
	float ExpThresholdMultiplier = 1.1f;

	UPROPERTY(Config, EditAnywhere)
	float ExperiencePointsForNextLvl = 20.f;
};
