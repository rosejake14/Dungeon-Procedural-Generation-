
// Fill out your copyright notice in the Description page of Project Settings.


#include "Floor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Math/Quat.h"
#include "Door.h"

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

	DoorPoints.Add(FTransform(FVector(0,0,0)));
}

// Called when the game starts or when spawned
void AFloor::BeginPlay()
{
	Super::BeginPlay();
	Origin.SetLocation(FVector3d(GetActorLocation()));
	SetDefaultValues();
	
	SpawnFloor(NumberOfXTiles, NumberOfYTiles);
	UE_LOG(LogTemp, Warning, TEXT("Room Spawned"))

	for (int i = 0; i < Iterations; i++)
	{
		Origin.SetLocation(FVector3d(DoorPoints[DoorPoints.Num() - 1].GetLocation()));
		Origin.SetRotation(-DoorPoints[DoorPoints.Num()].GetRotation());
		SetDefaultValues();
		UE_LOG(LogTemp, Warning, TEXT("Preparing to Spawn"))
		SpawnFloor(NumberOfXTiles, NumberOfYTiles);
		UE_LOG(LogTemp, Warning, TEXT("Room Spawned"))
	}
}

void AFloor::SetDefaultValues()
{
	NumberOfXTiles = FMath::RandRange(2, 10);
	NumberOfYTiles = FMath::RandRange(2, 10);
	MaxNumberOfDoors = FMath::RandRange(1, MaxNumberOfDoors);
	DoorsSpawned = 0;
	FirstWall = true;
	DoorPoints.Reset();
}

void AFloor::SpawnFloor(int x, int y)
{
	for (int i = 0; i < x; i++)
	{
		for (int j = 0; j < y; j++)
		{
			SpawnPoint.SetLocation(FVector3d(Origin.GetLocation().X + Length * i, Origin.GetLocation().Y + Width * j,
			                                 Origin.GetLocation().Z));
			FloorMesh->AddInstance(SpawnPoint);
			FloorPoints.Add(SpawnPoint);
			//SpawnPoint.SetLocation(FVector3d(Origin.GetLocation().X + Length*i,Origin.GetLocation().Y + Width*j,Origin.GetLocation().Z + Height));
			//FloorMesh->AddInstance(SpawnPoint);
		}
	}
	SpawnWall(NumberOfXTiles, NumberOfYTiles);
}

void AFloor::DetectType()
{
}



void AFloor::SpawnWall(int x, int y)
{
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	for (int i = 0; i < x; i++) // Spawn horizontal walls
	{
		FVector SpawnLocationTop = FVector(Origin.GetLocation().X + Length * i + Length, Origin.GetLocation().Y,
		                                   Origin.GetLocation().Z);
		FVector SpawnLocationBottom = FVector(Origin.GetLocation().X + Length * i, Origin.GetLocation().Y + Width * y,
		                                      Origin.GetLocation().Z);
		FQuat RotationTop = FQuat::MakeFromEuler(FVector(0, 0, 180));
		FQuat RotationBottom = FQuat::MakeFromEuler(FVector(0, 0, 0));
		bool SpawnTop = true; bool SpawnBottom = true;

		
		for (int k = 0; k < DoorPoints.Num(); k++)
		{
			if(FTransform(SpawnLocationTop).Equals(DoorPoints[k]) )
			{
				SpawnTop = false;
			}
			if(FTransform(SpawnLocationBottom).Equals(DoorPoints[k]))
			{
				SpawnBottom = false;
			}
		}
		
		
		
			if (DoorsSpawned <= MaxNumberOfDoors)
			{
				int randomNumber = rand() % 3;

				switch (randomNumber)
				{
				case 0:
					if(SpawnBottom)
					{
						WallMesh->AddInstance(FTransform(RotationBottom, SpawnLocationBottom));
					}
					if(SpawnTop)
					{
						GetWorld()->SpawnActor<ADoor>(Door, FTransform(RotationTop, SpawnLocationTop), SpawnParameters);
						DoorPoints.Add(FTransform(RotationTop, SpawnLocationTop));
						DoorsSpawned++;
					}
					break;
				case 1:
					if(SpawnTop)
					{
						WallMesh->AddInstance(FTransform(RotationTop, SpawnLocationTop));
					}
					if (SpawnBottom)
					{
						GetWorld()->SpawnActor<ADoor>(Door, FTransform(RotationBottom, SpawnLocationBottom), SpawnParameters);
						DoorPoints.Add(FTransform(RotationBottom, SpawnLocationBottom));
						DoorsSpawned++;
					}

					
					break;

				default:
					if(SpawnTop)
					{
						WallMesh->AddInstance(FTransform(RotationTop, SpawnLocationTop));
					}
					if (SpawnBottom)
					{
						WallMesh->AddInstance(FTransform(RotationBottom, SpawnLocationBottom));
					}
					break;
				}
			}
			else
			{
				if(SpawnTop)
				{
					WallMesh->AddInstance(FTransform(RotationTop, SpawnLocationTop));
				}
				if (SpawnBottom)
				{
					WallMesh->AddInstance(FTransform(RotationBottom, SpawnLocationBottom));
				}
			}
		
	}

	for (int j = 0; j < y; j++) // Spawn vertical walls
	{
		FVector SpawnLocationRight = FVector(Origin.GetLocation().X, Origin.GetLocation().Y + Width * j,
		                                     Origin.GetLocation().Z);
		FVector SpawnLocationLeft = FVector(Origin.GetLocation().X + Length * x, Origin.GetLocation().Y + Width * j + Width,
		                                    Origin.GetLocation().Z);
		
		// Use a quaternion to rotate the walls to be vertical
		FQuat RotationRight = FQuat::MakeFromEuler(FVector(0, 0, 90));
		FQuat RotationLeft = FQuat::MakeFromEuler(FVector(0, 0, -90));
		// 90-degree rotation around the Z-axis
		// if (FirstWall)
		// {
		// 	WallMesh->AddInstance(FTransform(Rotation, SpawnLocationLeft));
		// 	FirstWall = false;
		// }
		bool SpawnLeft = true; bool SpawnRight = true;

		
		for (int k = 0; k < DoorPoints.Num(); k++)
		{
			if(FTransform(SpawnLocationRight).Equals(DoorPoints[k]))
			{
				SpawnRight = false;
			}
			if(FTransform(SpawnLocationLeft).Equals(DoorPoints[k]))
			{
				SpawnLeft = false;
			}
		}
		
			if (DoorsSpawned <= MaxNumberOfDoors)
			{
				int randomNumber = rand() % 3;

				switch (randomNumber)
				{
				case 0:
					if(SpawnLeft)
					{
						WallMesh->AddInstance(FTransform(RotationLeft, SpawnLocationLeft));
					}
					if (SpawnRight)
					{
						GetWorld()->SpawnActor<ADoor>(Door, FTransform(RotationRight, SpawnLocationRight), SpawnParameters);
						DoorPoints.Add(FTransform(RotationRight, SpawnLocationRight));
						DoorsSpawned++;
					}
					break;
				case 1:
					if(SpawnLeft)
					{
						GetWorld()->SpawnActor<ADoor>(Door, FTransform(RotationLeft, SpawnLocationLeft), SpawnParameters);
                        DoorPoints.Add(FTransform(RotationLeft, SpawnLocationLeft));
                        DoorsSpawned++;
					}
					if (SpawnRight)
					{
						WallMesh->AddInstance(FTransform(RotationRight,SpawnLocationRight));
					}
					break;

				default:
					if(SpawnLeft)
					{
						WallMesh->AddInstance(FTransform(RotationLeft, SpawnLocationLeft));
					}
					if (SpawnRight)
					{
						WallMesh->AddInstance(FTransform(RotationRight, SpawnLocationRight));
					}
					break;
				}
			}
			else
			{
				if(SpawnLeft)
				{
					WallMesh->AddInstance(FTransform(RotationLeft, SpawnLocationLeft));
				}
				if (SpawnRight)
				{
					WallMesh->AddInstance(FTransform(RotationRight, SpawnLocationRight));
				}
			}
		
	}
}


// Called every frame
void AFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AFloor::HasCollision(const FVector& TestLocation)
{
    // Check for collision with existing rooms
    for (const FTransform& ExistingRoom : FloorPoints)
    {
        FVector ExistingRoomLocation = ExistingRoom.GetLocation();
        FVector Extent = FVector(Length / 2, Width / 2, Height / 2); // Adjust the extent based on your room dimensions

        if (FMath::Abs(ExistingRoomLocation.X - TestLocation.X) < Extent.X &&
            FMath::Abs(ExistingRoomLocation.Y - TestLocation.Y) < Extent.Y)
        {
            // Collision detected
            return true;
        }
    }

    return false;
}