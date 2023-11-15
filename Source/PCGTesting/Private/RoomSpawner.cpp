
#include "RoomSpawner.h"
#include "Components/InstancedStaticMeshComponent.h"


ARoomSpawner::ARoomSpawner()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	
	
}

void ARoomSpawner::OnConstruction(const FTransform& Transform)
{
	ISMComp = NewObject<UInstancedStaticMeshComponent>(this);
	ISMComp->RegisterComponent();
	ISMComp->SetFlags(RF_Transactional);
	this->AddInstanceComponent(ISMComp);
	
	WallISMComp = NewObject<UInstancedStaticMeshComponent>(this);
	WallISMComp->RegisterComponent();
	WallISMComp->SetFlags(RF_Transactional);
	this->AddInstanceComponent(WallISMComp);
	
	
	//Actor = GetWorld()->SpawnActor<AActor>(AActor::StaticClass());
}

void ARoomSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnFloors();
	SpawnLRWalls();
	
}

void ARoomSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARoomSpawner::SpawnFloors()
{
	for (int i = 0; i < NumberOfXTiles; i++)
	{
		for (int j = 0; j < NumberOfYTiles; j++)
		{
			ISMComp->SetStaticMesh(FloorMesh);
			SpawnLocation.SetLocation(FVector3d(GetActorLocation().X + 400 * i, GetActorLocation().Y + 400 * j,GetActorLocation().Z));
			ISMComp->AddInstance(SpawnLocation);
		}
	}
}

void ARoomSpawner::SpawnLRWalls()
{
	FQuat RotationLeft = FQuat::MakeFromEuler(FVector(0, 0, 0));
	FQuat RotationBottom = FQuat::MakeFromEuler(FVector(0, 0, 90));
	FQuat RotationRight = FQuat::MakeFromEuler(FVector(0, 0, 180));
	FQuat RotationTop = FQuat::MakeFromEuler(FVector(0, 0, -90));
	
	WallISMComp->SetStaticMesh(WallMesh);
	
	// Spawn left and right walls
	for (int i = 0; i < NumberOfYTiles; i++)
	{
		SpawnLocation = FTransform(RotationLeft, FVector(GetActorLocation().X, GetActorLocation().Y + 400 * i, GetActorLocation().Z));
		WallISMComp->AddInstance(SpawnLocation);
		SpawnLocation = FTransform(RotationRight, FVector(GetActorLocation().X + 400 * NumberOfXTiles, GetActorLocation().Y + 400 * i, GetActorLocation().Z));
		WallISMComp->AddInstance(SpawnLocation);
	}

	// Spawn top and bottom walls
	for (int i = 0; i < NumberOfXTiles; i++)
	{
		SpawnLocation = FTransform(RotationTop, FVector(GetActorLocation().X + 400 * i, GetActorLocation().Y, GetActorLocation().Z));
		WallISMComp->AddInstance(SpawnLocation);
		SpawnLocation = FTransform(RotationBottom, FVector(GetActorLocation().X + 400 * i, GetActorLocation().Y + 400 * NumberOfYTiles, GetActorLocation().Z));
		WallISMComp->AddInstance(SpawnLocation);
	}
}

