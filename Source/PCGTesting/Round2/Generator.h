// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Generator.generated.h"

UCLASS()
class PCGTESTING_API AGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGenerator();
	virtual void Tick(float DeltaTime) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;
	void GenerateMap();
	void MakeFloor(FVector Location, TArray<FVector> &FloorTilesIn, FVector &LocationOut, FVector &Extents);
	void SpawnTiles();
	void FindNextLocation(bool &Valid, FVector &NewLocationtoSpawn);
	void CreateCorridors(FVector RoomA, FVector RoomB, TArray<FVector>& CorridorTilesReturn);
	void SpawnCorridorsY(FVector Start, FVector Finish, TArray<FVector>& FloorTilesReturn);
	void SpawnCorridorsX(FVector Start, FVector Finish, TArray<FVector> &FloorTilesReturn);
	void TestRelativeLocation(FVector ReferenceLocation, TArray<FVector> TestArray, int32 X, int32 Y, FVector &Location, bool &isPresentTile);

	UPROPERTY(VisibleAnywhere)
	int32 Seed;
	UPROPERTY(EditAnywhere, Category = " ")
	int32 RoomSizeMin;
	UPROPERTY(EditAnywhere, Category = " ")
	int32 RoomSizeMax;
	UPROPERTY(EditAnywhere, Category = " ")
	int32 RoomCount;
	UPROPERTY(EditAnywhere)
	int32 FloorCullingMin; //Holes in Rooms
	UPROPERTY(EditAnywhere)
	int32 FloorCullingMax;
	UPROPERTY(EditAnywhere, Category = " ")
	bool isCulling;
	UPROPERTY(EditAnywhere)
	int32 BranchingThreshold;
	UPROPERTY(EditAnywhere);
	int32 MaxLoops = 10;
	UPROPERTY(EditAnywhere, Category = " ");
	bool NewSeed;
	UPROPERTY(VisibleAnywhere);
	FRandomStream Stream;
	UPROPERTY(EditAnywhere);
	float Scale;
	UPROPERTY(EditAnywhere)
	float BranchingChance;
	UPROPERTY(EditAnywhere)
	bool Merging;
	UPROPERTY(EditAnywhere)
	bool Branching;
	UPROPERTY(VisibleAnywhere)
	TArray<FVector> FloorTiles;
	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* FloorISMC;
	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* WallISMC;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root = nullptr;
	UPROPERTY(EditAnywhere)
	bool Working = true;
	UPROPERTY(VisibleAnywhere)
	FVector PreviousLocation;

	bool isFloorTile;
	bool ValidTileLocation = false;
	FVector NewLocation;
	UPROPERTY(VisibleAnywhere)
	TMap<FVector, FVector> Rooms;
	TArray<FVector> CorridorTiles;
	FVector NextLocation;

};


