#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "UWBoidSubsystem.generated.h"

class AUWSheep;

struct FBoid
{
	uint32 ID;
	FVector Position;
	FVector Velocity;
	uint16 SheepPoints;
};

struct FBoidNeighbour
{
	const FBoid* BoidRawPtr;
	float Distance;
};

UCLASS()
class UUWBoidSubsystem : public UTickableWorldSubsystem
{
public:
	GENERATED_BODY()
	UUWBoidSubsystem();
	
	//~ UWorldSubsystem
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	//~ ---
	
	//~ UTickableWorldSubsystem
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	// ---

	bool RegisterWolf(AActor* Wolf);
	bool RegisterSheep(AUWSheep* Sheep, uint32& RetID);
	bool UnregisterActor(uint32 ID);

protected:
	void GatherNeighbours(const FBoid& Boid);
	FVector ApplySeparation(const FBoid& Boid);
	FVector ApplyAlignment(const FBoid& Boid);
	FVector ApplyCohesion(const FBoid& Boid);
	FVector ApplyWolf(const FBoid& Boid);
	FVector LimitVectorLength(const FVector& Vector, float MaxSize);
	
private:
	uint32 NextID = 0;
	TArray<FBoid> Boids;
	TArray<FBoidNeighbour> CurrentNeighbours;
	FVector WolfPosition = FVector::ZeroVector;

	float NeighbourWolfRadius = 500.f;
	float NeighbourRadius = 300.f;
	float SeparationWeight = 1.3f;
	float AlignmentWeight = 1.2f;
	float CohesionWeight = 1.2f;
	float WolfWeight = 2.f;
	float MaxSpeed = 400.f;
	float MaxForce = 100.f;

	UPROPERTY()
	TMap<uint32, AActor*> BoidActors;

	UPROPERTY()
	TWeakObjectPtr<AActor> WolfWeakPtr = nullptr;
};
