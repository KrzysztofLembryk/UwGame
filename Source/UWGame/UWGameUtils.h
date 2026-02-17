#pragma once

#include "UWGameUtils.generated.h"

USTRUCT(BlueprintType)
struct FLevelRulesData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LevelNumber;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinScoreToNextLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LevelTimeSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> LevelPath;
};	

USTRUCT(BlueprintType)
struct FScoreRow
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerScore;
};
