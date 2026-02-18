#pragma once

#include "UWGameUtils.h"
#include "Engine/GameInstance.h"
#include "UWSaveGame.h"

#include "UWGameInstance.generated.h"

UCLASS()
class UUWGameInstance : public UGameInstance
{
public:
	GENERATED_BODY()

	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	float GetScoreSum() const;

	UFUNCTION(BlueprintCallable)
	float GetTimeLeftInLevel() const;

	void AddMoreTime(float TimeToAdd);

	UFUNCTION(BlueprintCallable)
	void StartNewGame();

	UFUNCTION(BlueprintCallable)
	void SetPlayerName(FString InPlayerName);

	UFUNCTION(BlueprintCallable)
	TArray<FScoreRow> GetAllScores() const;
	
	bool OpenLevel(int32 LevelNum);
	
	void AddScore(float ScoreToAdd);

	UFUNCTION(BlueprintCallable)
	void SubstractFromScore(float ScoreToSubstract);

	void FinishGame();
	
	void TimeOut();

	FLevelRulesData GetCurrentLevelRules() const;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OpenHighScoreMenu();
	
	void LoadGameData();
	
	void SaveGameData();
	
protected:
	UPROPERTY()
	FString PlayerName;

	UPROPERTY()
	float ScoreSum = 0.f;
	
	UPROPERTY()
	int32 CurrentLevelNum = 1;

	UPROPERTY()
	FLevelRulesData CurrentLevelRules;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable> GameSettingsTable = nullptr;

	UPROPERTY()
	TObjectPtr<UUWSaveGame> SaveGameObject = nullptr;
};
