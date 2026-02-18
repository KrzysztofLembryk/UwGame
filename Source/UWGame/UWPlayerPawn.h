#pragma once

#include "GameFramework/DefaultPawn.h"
#include "UWPlayerPawn.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;

UCLASS()
class AUWPlayerPawn : public ADefaultPawn
{
	GENERATED_BODY()
	
public:
	// Changing speed to acceleration per second, because now, ADefaultPawn gives us FloatingMovementComponent features with max speed defined there
	UPROPERTY(EditAnywhere, Category = "CharacterStatistics")
	float AccelerationPerSecond = 50.f;

	UPROPERTY(EditAnywhere, Category = "CharacterStatistics")
	float Damage = 2.f;

	UPROPERTY(EditAnywhere, Category = "CharacterStatistics")
	float CactusRetaliation = 1.5f;

	UPROPERTY(EditAnywhere, Category = "CharacterStatistics")
	float AdditionalTime = 10.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterStatistics")
	float ExperiencePoints = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterStatistics")
	int32 CurrCharacterLevel = 0;

	UPROPERTY(EditAnywhere, Category = "CharacterStatistics")
	float ExperiencePointsForNextLvl = 0.f;

	UPROPERTY(EditAnywhere, Category = "EI")
	TObjectPtr<UInputMappingContext> WolfInputMappingContext = nullptr;

	UPROPERTY(EditAnywhere, Category = "EI")
	TObjectPtr<UInputAction> MovementInputAction;

	UPROPERTY(EditAnywhere, Category = "EI")
	TObjectPtr<UInputAction> CameraInputAction;

	//~ AActor
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	//~ ---
	
	UFUNCTION(BlueprintCallable)
	bool ConsumeSheep(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void RecvCactusRetaliation();
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	void HandleMovementInput(const FInputActionValue& InputValue);
	void HandleCameraInput(const FInputActionValue& InputValue);

};

