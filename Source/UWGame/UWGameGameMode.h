#pragma once

#include "GameFramework/GameModeBase.h"
#include "UWGameGameMode.generated.h"

UCLASS()
class UWGAME_API AUWGameGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AUWGameGameMode();

	// AActor interface
	virtual void Tick(float DeltaSeconds) override;
	// ---
	
	//~ AGameModeBase interface
	virtual void InitGameState() override;
	// ---
};
