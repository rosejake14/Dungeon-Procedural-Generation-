// Fill out your copyright notice in the Description page of Project Settings.


#include "Floor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Math/Quat.h"
#include "Math/UnitConversion.h"
#include "Components/BoxComponent.h"
#include "PCGTesting/PCGTestingCharacter.h"

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

	DoorCollision = CreateDefaultSubobject<UBoxComponent>("Collider");
	DoorCollision->SetupAttachment(DoorMesh);
}

// Called when the game starts or when spawned
void AFloor::BeginPlay()
{
	Super::BeginPlay();
	Origin.SetLocation(FVector3d(GetActorLocation()));
	NumberOfXTiles = FMath::RandRange(2,10);
	NumberOfYTiles = FMath::RandRange(2,10);
	MaxNumberOfDoors = FMath::RandRange(1,(NumberOfXTiles + NumberOfYTiles)/3);

	DoorCollision->OnComponentBeginOverlap.AddDynamic(this, &AFloor::OnBoxOverlap);

	SpawnFloor(NumberOfXTiles, NumberOfYTiles);
}

void AFloor::SpawnFloor(int x, int y)
{
	for(int i = 0; i < x; i++)
	{
		for (int j = 0; j < y; j++)
		{
			
			SpawnPoint.SetLocation(FVector3d(Origin.GetLocation().X + Length*i,Origin.GetLocation().Y +Width*j,Origin.GetLocation().Z));
			FloorMesh->AddInstance(SpawnPoint);
			SpawnPoint.SetLocation(FVector3d(Origin.GetLocation().X + Length*i,Origin.GetLocation().Y + Width*j,Origin.GetLocation().Z + Height));
			FloorMesh->AddInstance(SpawnPoint);
		}
	}
	SpawnWall(NumberOfXTiles, NumberOfYTiles);
}

void AFloor::SpawnWall(int x, int y)
{
	for (int i = 0; i < x; i++) // Spawn horizontal walls
	{
		FVector SpawnLocationTop = FVector(Origin.GetLocation().X + Length * i,Origin.GetLocation().Y,Origin.GetLocation().Z);
		FVector SpawnLocationBottom = FVector(Origin.GetLocation().X + Length * i,Origin.GetLocation().Y + Width * y, Origin.GetLocation().Z);
		
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
		FVector SpawnLocationRight = FVector(Origin.GetLocation().X, Origin.GetLocation().Y + Width * j, Origin.GetLocation().Z);
		FVector SpawnLocationLeft = FVector(Origin.GetLocation().X + Length * x, Origin.GetLocation().Y + Width * j, Origin.GetLocation().Z);
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

void AFloor::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult)
{
	APCGTestingCharacter* Player = Cast<APCGTestingCharacter>(OtherActor);
	if(Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("New Room"));
	}
}


// Called every frame
void AFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

