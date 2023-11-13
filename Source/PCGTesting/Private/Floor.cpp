// Fill out your copyright notice in the Description page of Project Settings.


#include "Floor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Math/Quat.h"
#include "Math/UnitConversion.h"

// Sets default values
AFloor::AFloor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;
	
	FloorMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Floor");

	WallMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Wall");

	DoorMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Door");
}

// Called when the game starts or when spawned
void AFloor::BeginPlay()
{
	Super::BeginPlay();
	NumberOfXTiles = FMath::RandRange(2,10);
	NumberOfYTiles = FMath::RandRange(2,10);
	MaxNumberOfDoors = FMath::RandRange(1,(NumberOfXTiles + NumberOfYTiles)/3);
	SpawnFloor(NumberOfXTiles, NumberOfYTiles);
}

void AFloor::SpawnFloor(int x, int y)
{
	for(int i = 0; i < x; i++)
	{
		for (int j = 0; j < y; j++)
		{
			SpawnPoint.SetLocation(FVector3d(Length*i,Width*j,0));
			FloorMesh->AddInstance(SpawnPoint);
			SpawnPoint.SetLocation(FVector3d(Length*i,Width*j,Height));
			FloorMesh->AddInstance(SpawnPoint);
		}
	}
	SpawnWall(NumberOfXTiles, NumberOfYTiles);
}

void AFloor::SpawnWall(int x, int y)
{
	for (int i = 0; i < x; i++) // Spawn horizontal walls
	{
		FVector SpawnLocationTop = FVector(Length * i, 0, 0);
		FVector SpawnLocationBottom = FVector(Length * i, Width * y, 0);
		
		if (DoorsSpawned <= MaxNumberOfDoors)
		{
			int randomNumber = rand() % 3;
			
			switch (randomNumber)
			{
				case 0:
					DoorMesh->AddInstance(FTransform(SpawnLocationTop));
					WallMesh->AddInstance(FTransform(SpawnLocationBottom));
					DoorsSpawned++;
					break;
				case 1:
					WallMesh->AddInstance(FTransform(SpawnLocationTop));
					DoorMesh->AddInstance(FTransform(SpawnLocationBottom));
					DoorsSpawned++;
					break;

				default:
					WallMesh->AddInstance(FTransform(SpawnLocationTop));
					WallMesh->AddInstance(FTransform(SpawnLocationBottom));
					break; 
			}
			
		}
		else
		{
			WallMesh->AddInstance(FTransform(SpawnLocationTop));
			WallMesh->AddInstance(FTransform(SpawnLocationBottom));
		}
		
	}

	for (int j = 0; j < y; j++)	// Spawn vertical walls
	{
		FVector SpawnLocationRight = FVector(0, Width * j, 0);
		FVector SpawnLocationLeft = FVector(Length * x, Width * j, 0);
		// Use a quaternion to rotate the walls to be vertical
		FQuat Rotation = FQuat::MakeFromEuler(FVector(0, 0, 90)); // 90-degree rotation around the Z-axis
		
		if (DoorsSpawned <= MaxNumberOfDoors)
		{
			int randomNumber = rand() % 3;
			
			switch (randomNumber)
			{
			case 0:
				DoorMesh->AddInstance(FTransform(Rotation,SpawnLocationRight));
				WallMesh->AddInstance(FTransform(Rotation,SpawnLocationLeft));
				DoorsSpawned++;
				break;
			case 1:
				WallMesh->AddInstance(FTransform(Rotation,SpawnLocationRight));
				DoorMesh->AddInstance(FTransform(Rotation,SpawnLocationLeft));
				DoorsSpawned++;
				break;

			default:
				WallMesh->AddInstance(FTransform(Rotation,SpawnLocationRight));
				WallMesh->AddInstance(FTransform(Rotation,SpawnLocationLeft));
				break; 
			}
			
		}
		else
		{
			WallMesh->AddInstance(FTransform(Rotation,SpawnLocationRight));
			WallMesh->AddInstance(FTransform(Rotation,SpawnLocationLeft));
		}
	}
}


// Called every frame
void AFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

