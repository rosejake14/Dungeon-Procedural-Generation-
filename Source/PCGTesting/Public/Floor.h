
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floor.generated.h"

class UInstancedStaticMeshComponent;
class ADoor;

UCLASS()
class PCGTESTING_API AFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	AFloor();
	virtual void Tick(float DeltaTime) override;
	void SpawnProceduralRoom();
	bool HasCollision(const FVector& TestLocation);

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

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Door;
	
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

	UFUNCTION()
	void SpawnFloor(int x, int y);

	UFUNCTION()
	void DetectType();


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

	UPROPERTY(VisibleAnywhere)
	TArray<FTransform> DoorPoints;

	UPROPERTY(VisibleAnywhere)
	TArray<FTransform> FloorPoints;

	UPROPERTY(EditAnywhere)
	int Iterations = 3;
	UFUNCTION()
	void SetDefaultValues();

	bool FirstWall = true;

	
private:


};
