#include "UWPlayerPawn.h"
#include "UWGameLog.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "UWBoidSubsystem.h"
#include "UWGameGameMode.h"
#include "UWGameGameState.h"
#include "UWGameInstance.h"
#include "UWGameSettings.h"
#include "UWSheep.h"
#include "Kismet/KismetMathLibrary.h"

void AUWPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	UUWBoidSubsystem* BoidSubsystem = GetWorld()->GetSubsystem<UUWBoidSubsystem>();
	if ( ! BoidSubsystem)
	{
		return;
	}

	BoidSubsystem->RegisterWolf(this);
}

void AUWPlayerPawn::Destroyed()
{
	Super::Destroyed();
}

bool AUWPlayerPawn::ConsumeSheep(AActor* Actor)
{
	if (AUWSheep* Sheep = Cast<AUWSheep>(Actor))
	{
		if (Sheep->CanBeEaten())
		{
			if (Sheep->ReceiveDamage(this->Damage))
			{
				Sheep->Destroy();
				
				if (UUWGameInstance* GameInst = Cast<UUWGameInstance>(GetGameInstance()))
				{
					int32 SheepPoints = Sheep->GetSheepPoints();
					GameInst->AddScore(SheepPoints);
					ExperiencePoints += float(SheepPoints);

					const UUWGameSettings* Settings = GetDefault<UUWGameSettings>();

					while (ExperiencePoints >= ExperiencePointsForNextLvl)
					{
						ExperiencePoints -= ExperiencePointsForNextLvl;
						CurrCharacterLevel++;

						if (ExperiencePointsForNextLvl <= 0.f)
						{
							ExperiencePoints = Settings->ExperiencePointsForNextLvl;
						}
						ExperiencePointsForNextLvl *= Settings->ExpThresholdMultiplier;

						// Uniformly randomly choose one stat to upgrade 
                        int32 RandomChoice = FMath::RandRange(0, 3);
                        
                        switch (RandomChoice)
                        {
                        case 0:
                            AccelerationPerSecond *= Settings->SpeedMultiplier;

                            // UE_LOG(LogUwGame, Log, TEXT("Level Up! Speed increased by %.2f"), Settings->SpeedMultiplier);

							GEngine->AddOnScreenDebugMessage(
								-1, 
								6.f, 
								FColor::Yellow, 
								FString::Printf(
									TEXT("Level Up! Speed: %.2f"), AccelerationPerSecond),
								true,
								FVector2D(2.0f, 2.0f)
								);
                            break;
                        case 1:
                            Damage *= Settings->DamageMultiplier;

                            // UE_LOG(LogUwGame, Log, TEXT("Level Up! Damage increased by %.2f"), Settings->DamageMultiplier);

							GEngine->AddOnScreenDebugMessage(
								-1,
								5.f,
								FColor::Yellow,
								FString::Printf(
									TEXT("Level Up! Damage: %.2f"), Damage),
								true,
								FVector2D(2.0f, 2.0f)
							);

                            break;
                        case 2:
                            CactusRetaliation *= Settings->CactusRetaliationMultiplier;

                            // UE_LOG(LogUwGame, Log, TEXT("Level Up! Cactus Retaliation decreased by %.2f"), Settings->CactusRetaliationMultiplier);

							GEngine->AddOnScreenDebugMessage(
								-1,
								5.f,
								FColor::Yellow,
								FString::Printf(
									TEXT("Level Up! Cactus Retaliation: %.2f"), 
									CactusRetaliation),
								true,
								FVector2D(2.0f, 2.0f)
							);
                            break;
                        case 3:
                            AdditionalTime *= Settings->AdditionalTimeMultiplier;
							GameInst->AddMoreTime(AdditionalTime);

                            // UE_LOG(LogUwGame, Log, TEXT("Level Up! Added %.2f seconds till the end of game"), AdditionalTime);

							GEngine->AddOnScreenDebugMessage(
								-1,
								5.f,
								FColor::Yellow,
								FString::Printf(
									TEXT("Level Up! Added Time: %.2f seconds"),
									AdditionalTime),
								true,
								FVector2D(2.0f, 2.0f)
							);
                            break;
                        }
					}
				}
	
				return true;
			}
			else
			{
				UE_LOG(LogUwGame, Log, TEXT("Current Sheep health: %d, dealt damage: %f"), Sheep->GetSheepHealth(), this->Damage);
				return false;
			}
		}
	}

	return false;
}

void AUWPlayerPawn::RecvCactusRetaliation()
{
	ExperiencePoints -= this->CactusRetaliation * this->Damage;

	if (ExperiencePoints < 0.f)
	{
		ExperiencePoints = 0.f;
	}
}

void AUWPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if ( ! PlayerController)
	{
		UE_LOG(LogUwGame, Error, TEXT("Player Pawn does not have player controller"));
		return;
	}

	// Adding InputMappingContext asset to EI subsystem (enabling it)
	UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if ( ! EISubsystem)
	{
		UE_LOG(LogUwGame, Error, TEXT("UEnhancedInputLocalPlayerSubsystem is null"));
		return;
	}

	if ( ! WolfInputMappingContext || ! MovementInputAction || ! CameraInputAction)
	{
		UE_LOG(LogUwGame, Error, TEXT("WolfInputMappingContext is null or MovementInputAction is null or CameraInputAction is null"));
		return;
	}
	
	EISubsystem->AddMappingContext(WolfInputMappingContext, 0);
	
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if ( ! EIC)
	{
		UE_LOG(LogUwGame, Error, TEXT("PlayerInputComponent is not UEnhancedInputComponent type. Enable EnhancedInput subsystem in settings."));
		return;
	}
	
	EIC->BindAction(MovementInputAction, ETriggerEvent::Triggered, this, &AUWPlayerPawn::HandleMovementInput);
	EIC->BindAction(CameraInputAction, ETriggerEvent::Triggered, this, &AUWPlayerPawn::HandleCameraInput);
}

void AUWPlayerPawn::HandleMovementInput(const FInputActionValue& InputValue)
{
	const EInputActionValueType InputType = InputValue.GetValueType();

	switch (InputType)
	{
	case EInputActionValueType::Axis2D:
		{
			float DeltaTimeSeconds = GetWorld()->GetDeltaSeconds();
			FInputActionValue::Axis2D XYDelta = InputValue.Get<FInputActionValue::Axis2D>();
			XYDelta = XYDelta * DeltaTimeSeconds * AccelerationPerSecond;

			FVector ForwardVector = UKismetMathLibrary::GetForwardVector(GetController()->GetControlRotation());
			ForwardVector.Normalize();

			FVector RightVector = UKismetMathLibrary::GetRightVector(GetController()->GetControlRotation());
			RightVector.Normalize();

			ForwardVector *= XYDelta.X; // scale forward vector 
			RightVector *= XYDelta.Y; // scale right vector

			AddMovementInput(ForwardVector, 1.f, false);
			AddMovementInput(RightVector, 1.f, false);

			FVector CharacterDirection = ForwardVector + RightVector;
			CharacterDirection.Z = 0.f;
			CharacterDirection.Normalize();
			
			SetActorRotation(CharacterDirection.Rotation());
			break;
		}
	default:
		break;
	}
}

void AUWPlayerPawn::HandleCameraInput(const FInputActionValue& InputValue)
{
	const EInputActionValueType InputType = InputValue.GetValueType();

	switch (InputType)
	{
	case EInputActionValueType::Axis2D:
		{
			FInputActionValue::Axis2D MouseXYDelta = InputValue.Get<FInputActionValue::Axis2D>();
			AddControllerYawInput(MouseXYDelta.X);
			AddControllerPitchInput(-MouseXYDelta.Y);
			break;
		}
	default:
		break;
	}
}

