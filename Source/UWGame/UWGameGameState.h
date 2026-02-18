#pragma once

#include "GameFramework/GameState.h"
#include "UWGameGameState.generated.h"

UCLASS()
class UWGAME_API AUWGameGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AUWGameGameState();

	float GetTimeLeftSeconds() const;
	void AddMoreTimeSeconds(float TimeToAdd);
	float GetCurrentScore() const;
	
protected:
	UPROPERTY()
	float TimeLeftSeconds = 0.f;

	UPROPERTY()
	float CurrentScore = 0.f;
	
	UPROPERTY()
	bool bTimeOut = false;
	
	friend class AUWGameGameMode;
};
