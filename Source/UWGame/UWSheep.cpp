#include "UWSheep.h"

#include "UWBoidSubsystem.h"

void AUWSheep::BeginPlay()
{
	Super::BeginPlay();

	UUWBoidSubsystem* BoidSubsystem = GetWorld()->GetSubsystem<UUWBoidSubsystem>();
	if ( ! BoidSubsystem)
	{
		return;
	}

	BoidSubsystem->RegisterSheep(this, BoidID);
	OnSheepAloneStateChange(bIsAlone);
}

void AUWSheep::Destroyed()
{
	Super::Destroyed();
	
	UUWBoidSubsystem* BoidSubsystem = GetWorld()->GetSubsystem<UUWBoidSubsystem>();
	if ( ! BoidSubsystem)
	{
		return;
	}
	
	BoidSubsystem->UnregisterActor(BoidID);
}

void AUWSheep::SetIsSheepAlone(bool bInIsAlone)
{
	if (bIsAlone != bInIsAlone)
	{
		bIsAlone = bInIsAlone;
		OnSheepAloneStateChange(bIsAlone);
	}
}

bool AUWSheep::CanBeEaten() const
{
	return bIsAlone;
}


bool AUWSheep::ReceiveDamage(int32 DamageAmount)
{
	SheepHealth -= DamageAmount;
	SetSheepHealth(SheepHealth);
	
	return SheepHealth <= 0;
}

void AUWSheep::SetSheepHealth(int32 NewValue)
{
	SheepHealth = NewValue;
	
	OnSheepHealthChanged(SheepHealth);
}

void AUWSheep::SetSheepPoints(int32 NewValue)
{
	SheepPoints = NewValue;
	
	OnSheepPointsChanged(SheepPoints);
}

int32 AUWSheep::GetSheepPoints() const
{
	return SheepPoints;
}
