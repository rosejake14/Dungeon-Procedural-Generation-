#include "PCGTesting/Round2/ActorSpawner.h"
#include "PCGTesting/Round2/Generator.h"
#include "Kismet/KismetMathLibrary.h"

UActorSpawner::UActorSpawner()
{
	
	PrimaryComponentTick.bCanEverTick = true;

}

void UActorSpawner::BeginPlay()
{
	Super::BeginPlay();
	SpawnItems();
}

void UActorSpawner::SpawnItems()
{
	AActor* OwningActor = GetOwner();
	if(OwningActor)
	{
		//Code for using the Generator Reference goes here...
		GeneratorRef = Cast<AGenerator>(OwningActor);
		FloorTilesCopy = GeneratorRef->FloorTiles;
		if(RoomSpawnOnly)
		{
			//To do...
		}else
		{
			FloorTilesCopy.Append(GeneratorRef->CorridorTiles);
		}
		
		CreateInstances();
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast to Owning Actor Failed!"))
	}
}

void UActorSpawner::CreateInstances()
{
	int32 ItemsSpawned = 0;
	int32 FloorIndex;
	FActorSpawnParameters SpawnParameters;
	FTransform LocationToSpawn;
	FVector LastLocation;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	while(Quantity > ItemsSpawned)
	{ //If the amount of spawned items isn't met the desired amount then loop.
		//Gets a random floor with reference to stream.
		FloorIndex = GeneratorRef->Stream.RandRange(0,FloorTilesCopy.Num() - 1); 
		while(UKismetMathLibrary::EqualEqual_Vector4Vector4(FloorTilesCopy[FloorIndex] * GeneratorRef->Scale ,LastLocation, MinDistance))
		{
			// Ensure the newly chosen floor is not too close too the last spawned location.
			FloorIndex = GeneratorRef->Stream.RandRange(0,FloorTilesCopy.Num() - 1);
		}
		
		if(!UKismetMathLibrary::EqualEqual_Vector4Vector4(FloorTilesCopy[FloorIndex] * GeneratorRef->Scale ,LastLocation, MinDistance))
		{
			// Get spawn location from a randomly chosen tile index.
			LocationToSpawn.SetLocation(FVector((FloorTilesCopy[FloorIndex] * GeneratorRef->Scale) + GeneratorRef->GetActorLocation() + Offset)); 

			// Save the last location without an offset on that tile.
			LastLocation = FVector((FloorTilesCopy[FloorIndex] * GeneratorRef->Scale) + GeneratorRef->GetActorLocation()); 
			for (auto Element : SpawnList)
			{
				// Iterate through the SpawnList and spawn an actor based on random weights.
				if(RandomBoolWithWeightFromStream(SpawnList.FindRef(Element.Key)))
					// Spawns an actor from the TMap which specifies actor and chance to spawn.
				{
					//Spawn AActor from TMap, Location and 'AdjustIfPossibleButAlwaysSpawn'. 
					GetWorld()->SpawnActor<AActor>(Element.Key, LocationToSpawn, SpawnParameters);
					
					//Assures another item can't spawn in the same location.
					FloorTilesCopy.RemoveAt(FloorIndex); 
					ItemsSpawned++;
					break;
				}
			}
		}
	}
	
}

bool UActorSpawner::RandomBoolWithWeightFromStream(float Weight)
{
	Weight = FMath::Clamp(Weight, 0, 1);

	float RandomValue = GeneratorRef->Stream.FRandRange(0,1);

	return RandomValue < Weight;
}