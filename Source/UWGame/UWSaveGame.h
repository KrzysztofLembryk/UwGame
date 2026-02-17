#pragma once

#include "CoreMinimal.h"
#include "UWGameUtils.h"
#include "GameFramework/SaveGame.h"

#include "UWSaveGame.generated.h"

UCLASS(BlueprintType)
class UUWSaveGame : public USaveGame
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FScoreRow> Scores;
};
