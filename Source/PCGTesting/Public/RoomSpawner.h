#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomSpawner.generated.h"

class UInstancedStaticMeshComponent;

UCLASS()
class PCGTESTING_API ARoomSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ARoomSpawner();
	void OnConstruction(const FTransform& Transform);
	virtual void Tick(float DeltaTime) override;
	void SpawnFloors();
	void SpawnLRWalls();
	void SpawnRightWalls();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UStaticMesh* FloorMesh;
	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* ISMComp;
	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* WallISMComp;
	UPROPERTY(EditAnywhere)
	int NumberOfXTiles = 1;
	UPROPERTY(EditAnywhere)
	int NumberOfYTiles = 1;
	UPROPERTY(EditAnywhere)
	UStaticMesh* WallMesh;
	UPROPERTY(EditAnywhere)
	UStaticMesh* DoorMesh;
	
	FTransform SpawnLocation;
};
