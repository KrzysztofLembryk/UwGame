#include "UWBoidSubsystem.h"

#include "UWGameLog.h"
#include "UWGameSettings.h"
#include "UWSheep.h"

UUWBoidSubsystem::UUWBoidSubsystem()
{
}

void UUWBoidSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	const UUWGameSettings* Settings = GetDefault<UUWGameSettings>();
	NeighbourRadius = Settings->NeighbourRadius;
	NeighbourWolfRadius = Settings->NeighbourWolfRadius;
	SeparationWeight = Settings->SeparationWeight;
	AlignmentWeight = Settings->AlignmentWeight;
	CohesionWeight = Settings->CohesionWeight;
	WolfWeight = Settings->WolfWeight;
	MaxSpeed = Settings->MaxSpeed;
	MaxForce = Settings->MaxForce;
}

void UUWBoidSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	const UUWGameSettings* Settings = GetDefault<UUWGameSettings>();
	
	WolfPosition = WolfWeakPtr.IsValid() ? WolfWeakPtr->GetActorLocation() : FVector::ZeroVector;
	WolfPosition.Z = 0.f;
	
	TArray<FBoid> CopyBoids= Boids;
	
	for (int32 i = 0; i < CopyBoids.Num(); ++i)
	{
		FBoid& Boid = CopyBoids[i];

		GatherNeighbours(Boid);
		FVector SeparationForce = ApplySeparation(Boid) * SeparationWeight;
		FVector AlignmentForce = ApplyAlignment(Boid) * AlignmentWeight;
		FVector CohesionForce = ApplyCohesion(Boid) * CohesionWeight;
		FVector WolfForce = ApplyWolf(Boid) * WolfWeight;
		
		FVector Acceleration = SeparationForce + AlignmentForce + CohesionForce + WolfForce;
		Acceleration = LimitVectorLength(Acceleration, MaxForce);
		
		Boid.Velocity += Acceleration * DeltaTime;
		Boid.Velocity = LimitVectorLength(Boid.Velocity, MaxSpeed);
		
		Boid.Position += Boid.Velocity * DeltaTime;
		
		if (AActor** BoidActor = BoidActors.Find(Boid.ID))
		{
			if (AUWSheep* Sheep = Cast<AUWSheep>(*BoidActor))
			{
				Sheep->SetIsSheepAlone(CurrentNeighbours.Num() == 0);
			}
			
			FHitResult HitResult;
			const FVector SheepLocation = (*BoidActor)->GetActorLocation();
			FVector NewSheepLocation = SheepLocation;
			NewSheepLocation.X = Boid.Position.X;
			NewSheepLocation.Y = Boid.Position.Y;
			
			(*BoidActor)->SetActorLocation(NewSheepLocation, true, &HitResult);
			(*BoidActor)->SetActorRotation(Boid.Velocity.Rotation());

			if (Settings->bDrawDebugs)
			{
				DrawDebugSphere( GetWorld(), Boid.Position, 30.f, 32, FColor::Red, false, 0.0f );
				DrawDebugCircle( GetWorld(), SheepLocation, NeighbourRadius, 50, FColor::Green, false, -1.f, 0, 0.f, FVector(0.f, 1.f, 0.f), FVector(1.f, 0.f, 0.f), true);	
			}

			if (HitResult.bBlockingHit)
			{
				Boid.Position = (*BoidActor)->GetActorLocation();
				Boid.Position.Z = 0.f;
				Boid.Velocity = HitResult.ImpactNormal;
				Boid.Velocity.Z = 0.f;
				Boid.Velocity *= MaxSpeed / 2;
			}
		}
	}

	// Update boids data in next frame
	Boids = CopyBoids;
	
}

TStatId UUWBoidSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UUWBoidSubsystem, STATGROUP_Tickables);
}

bool UUWBoidSubsystem::RegisterWolf(AActor* Wolf)
{
	if (Wolf != nullptr)
	{
		WolfWeakPtr = Wolf;
		return true;
	}

	return false;
}

bool UUWBoidSubsystem::RegisterSheep(AUWSheep* AuwSheep, uint32& RetID)
{
	if (AuwSheep == nullptr)
	{
		return false;
	}
	
	FBoid NewBoid;
	NewBoid.ID = ++NextID;
	NewBoid.Position = AuwSheep->GetActorLocation();
	NewBoid.Position.Z = 0.f;
	NewBoid.Velocity = FVector(FMath::RandRange(-100, 100), FMath::RandRange(-100, 100), 0.f);

	const UUWGameSettings* Settings = GetDefault<UUWGameSettings>();

	NewBoid.SheepPoints = FMath::RandRange(
		Settings->SheepPointsMin, 
		Settings->SheepPointsMax
	);
	NewBoid.SheepHealth = NewBoid.SheepPoints;

	AuwSheep->SetSheepPoints(NewBoid.SheepPoints);
	AuwSheep->SetSheepHealth(NewBoid.SheepHealth);
	
	Boids.Add(NewBoid);
	BoidActors.Add(NextID, AuwSheep);

	RetID = NextID;
	return true;
}

bool UUWBoidSubsystem::UnregisterActor(uint32 ID)
{
	BoidActors.Remove(ID);
	int32 IndexToRemove = Boids.IndexOfByPredicate([ID](const FBoid& Value)
	{
		return Value.ID == ID;
	});

	if (IndexToRemove != INDEX_NONE)
	{
		Boids.RemoveAtSwap(IndexToRemove);
		return true;
	}
	
	return false;
}

void UUWBoidSubsystem::GatherNeighbours(const FBoid& Boid)
{
	CurrentNeighbours.Reset();
	
	for (const FBoid& OtherBoid : Boids)
	{
		if (OtherBoid.ID == Boid.ID)
		{
			continue;
		}

		float Distance = FVector::Distance(Boid.Position, OtherBoid.Position);
		if (Distance < NeighbourRadius)
		{
			FBoidNeighbour Neighbour;
			Neighbour.Distance = Distance;
			Neighbour.BoidRawPtr = &OtherBoid;
			
			CurrentNeighbours.Add(Neighbour);
		}
	}
}

FVector UUWBoidSubsystem::ApplySeparation(const FBoid& Boid)
{
	FVector SteeringForce = FVector::ZeroVector;
    
	for (const FBoidNeighbour& Neighbour : CurrentNeighbours)
	{
		const FBoid& OtherBoid = *Neighbour.BoidRawPtr;
		float Distance = Neighbour.Distance;
		
		// repulsion force is stronger if closer to another boid
		FVector RepulsionDirection = Boid.Position - OtherBoid.Position;
        
		if (!RepulsionDirection.IsNearlyZero())
		{
			RepulsionDirection.Normalize();

			RepulsionDirection *= (NeighbourRadius / FMath::Max(Distance, 1.0f));
			SteeringForce += RepulsionDirection;
		}
	}
    
	if (CurrentNeighbours.Num() > 0)
	{
		SteeringForce /= CurrentNeighbours.Num();
        
		if (!SteeringForce.IsNearlyZero())
		{
			SteeringForce.Normalize();
			SteeringForce *= MaxSpeed;
			SteeringForce -= Boid.Velocity;
			SteeringForce = LimitVectorLength(SteeringForce, MaxForce);
		}
	}
    
	return SteeringForce;

}

FVector UUWBoidSubsystem::ApplyAlignment(const FBoid& Boid)
{
	FVector AverageVelocity = FVector::ZeroVector;
    
	for (const FBoidNeighbour& Neighbour : CurrentNeighbours)
	{
		const FBoid& OtherBoid = *Neighbour.BoidRawPtr;

		AverageVelocity += OtherBoid.Velocity;
	}
    
	if (CurrentNeighbours.Num() > 0)
	{
		AverageVelocity /= CurrentNeighbours.Num();
        
		if (!AverageVelocity.IsNearlyZero())
		{
			AverageVelocity.Normalize();
			AverageVelocity *= MaxSpeed;
            
			FVector SteeringForce = AverageVelocity - Boid.Velocity;
			SteeringForce = LimitVectorLength(SteeringForce, MaxForce);
			return SteeringForce;
		}
	}
    
	return FVector::ZeroVector;
}

FVector UUWBoidSubsystem::ApplyCohesion(const FBoid& Boid)
{
	FVector CenterOfMass = FVector::ZeroVector;
    
	for (const FBoidNeighbour& Neighbour : CurrentNeighbours)
	{
		const FBoid& OtherBoid = *Neighbour.BoidRawPtr;
		
		CenterOfMass += OtherBoid.Position;
	}
    
	if (CurrentNeighbours.Num() > 0)
	{
		CenterOfMass /= CurrentNeighbours.Num();
        
		FVector DesiredVelocity = CenterOfMass - Boid.Position;
        
		if (!DesiredVelocity.IsNearlyZero())
		{
			DesiredVelocity.Normalize();
			DesiredVelocity *= MaxSpeed;
            
			FVector SteeringForce = DesiredVelocity - Boid.Velocity;
			SteeringForce = LimitVectorLength(SteeringForce, MaxForce);
			return SteeringForce;
		}
	}
    
	return FVector::ZeroVector;
}

FVector UUWBoidSubsystem::ApplyWolf(const FBoid& Boid)
{
	float Distance = FVector::Distance(Boid.Position, WolfPosition);

	if (Distance < NeighbourWolfRadius)
	{
		FVector SteeringForce = Boid.Position - WolfPosition;
		SteeringForce.Normalize();
		SteeringForce *= MaxSpeed;
		// SteeringForce = LimitVectorLength(SteeringForce, MaxForce);
		
		return SteeringForce;
	}

	return FVector::ZeroVector;
}

FVector UUWBoidSubsystem::LimitVectorLength(const FVector& Vector, float MaxSize)
{
	if (Vector.SizeSquared() > FMath::Square(MaxSize))
	{
		return Vector.GetSafeNormal() * MaxSize;
	}
    
	return Vector;
}
