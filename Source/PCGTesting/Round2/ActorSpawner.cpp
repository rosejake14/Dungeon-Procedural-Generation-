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
	{
		FloorIndex = GeneratorRef->Stream.RandRange(0,FloorTilesCopy.Num() - 1);
		while(UKismetMathLibrary::EqualEqual_Vector4Vector4(FloorTilesCopy[FloorIndex] * GeneratorRef->Scale ,LastLocation, MinDistance))
		{
			FloorIndex = GeneratorRef->Stream.RandRange(0,FloorTilesCopy.Num() - 1);
		}
		
		if(!UKismetMathLibrary::EqualEqual_Vector4Vector4(FloorTilesCopy[FloorIndex] * GeneratorRef->Scale ,LastLocation, MinDistance))
		{
			LocationToSpawn.SetLocation(FVector((FloorTilesCopy[FloorIndex] * GeneratorRef->Scale) + GeneratorRef->GetActorLocation() + Offset)); //Get Spawn Location From Random Tile Index.
			LastLocation = FVector((FloorTilesCopy[FloorIndex] * GeneratorRef->Scale) + GeneratorRef->GetActorLocation()); //Saves Last Location without Offset on that tile.
			for (auto Element : SpawnList)
			{
				if(RandomBoolWithWeightFromStream(SpawnList.FindRef(Element.Key)))
				{
					GetWorld()->SpawnActor<AActor>(Element.Key, LocationToSpawn, SpawnParameters);
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