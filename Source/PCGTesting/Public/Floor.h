#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floor.generated.h"

class UBoxComponent;
class UInstancedStaticMeshComponent;

UCLASS()
class PCGTESTING_API AFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	AFloor();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	int NumberOfXTiles = 1;

	UPROPERTY(EditAnywhere)
	int NumberOfYTiles = 1;

	UPROPERTY(EditAnywhere)
	float Length = 400.f;

	UPROPERTY(EditAnywhere)
	float Width = 400.f;

	UPROPERTY(EditAnywhere)
	float Height = 400.f;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* FloorMesh;

	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* WallMesh;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root = nullptr;

	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* DoorMesh;

	UPROPERTY(EditAnywhere)
	UBoxComponent* DoorCollision;

	UFUNCTION()
	void SpawnFloor(int x, int y);

	UFUNCTION()
	void SpawnWall(int x, int y);

	UPROPERTY(VisibleAnywhere)
	FTransform SpawnPoint;

	UPROPERTY(VisibleAnywhere)
	FTransform Origin;

	UPROPERTY(EditAnywhere)
	int MaxNumberOfDoors = 10;

	UPROPERTY(VisibleAnywhere)
	int DoorsSpawned = 0;

	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep,
							  const FHitResult& SweepResult);
private:


};
