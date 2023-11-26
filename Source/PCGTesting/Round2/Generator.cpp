// Fill out your copyright notice in the Description page of Project Settings.


#include "PCGTesting/Round2/Generator.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Math/RandomStream.h"

// Sets default values
AGenerator::AGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	FloorISMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>("FloorISMC");
	FloorISMC->SetupAttachment(Root);

	WallISMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>("WallISMC");
	WallISMC->SetupAttachment(Root);
	
	
}

// Called when the game starts or when spawned
void AGenerator::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void AGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGenerator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	FloorISMC->ClearInstances();
	WallISMC->ClearInstances();
	PreviousLocation = FVector(0,0,0);
	
	if(NewSeed == true) //On click randomise and set new seed;
	{
		NewSeed = false;
		Seed = rand () % 90000;
		Stream.Initialize(Seed);
	}
	Stream.Initialize(Seed); //Manually update seed;
	GenerateMap();
}

void AGenerator::GenerateMap()
{
	
	FVector LocationOut;
	FVector Extents;
	TArray<FVector> FloorTilesIn;
	UE_LOG(LogTemp, Warning, TEXT("Generating Map..."));
	for(int i = 0; i < RoomCount; i++)
	{
		if(i == 0) //Generates first location of Actor 0,0,0.
		{
			MakeFloor(PreviousLocation, FloorTilesIn, LocationOut, Extents);
			FloorTiles = FloorTilesIn;
			Rooms.Add(LocationOut, Extents);
			UE_LOG(LogTemp, Warning, TEXT("Room Added"));
		}else
		{
			bool Valid;
			FVector NextLocation;
			FindNextLocation(Valid, NextLocation);
			if(Valid == true)
			{
				MakeFloor(NextLocation, FloorTilesIn, LocationOut, Extents);
				FloorTiles.Append(FloorTilesIn);
				Rooms.Add(LocationOut, Extents);
				UE_LOG(LogTemp, Warning, TEXT("Another Room Added"));
				PreviousLocation = NextLocation;
			}
		}
	}
	SpawnTiles();
}

void AGenerator::MakeFloor(FVector Location, TArray<FVector> &FloorTilesIn, FVector &LocationOut, FVector &Extents) 
{
	//Get Random X and Y Lengths & Make array of Floor Tiles from Root Position.
	FVector RootLocation = Location;
	int32 OutX = Stream.RandRange(RoomSizeMin, RoomSizeMax);
	int32 OutY = Stream.RandRange(RoomSizeMin, RoomSizeMax);
	Working = true;
	int32 Area = (OutX * OutY) - 1;
	TArray<FVector> Tiles;
	TArray<int32> ExtentsX;
	TArray<int32> ExtentsY;
	TArray<FVector> ConnectedTiles;
	TArray<FVector> TilesCopy;
	bool TileFound = true;
	int32 LoopCount = 0;
	
	UE_LOG(LogTemp, Warning, TEXT("Adding Tiles..."));
	for(int i = 0; i <= Area; i++)
	{
		Tiles.Add(FVector((i / OutY) + RootLocation.X, (i % OutY) + RootLocation.Y, RootLocation.Z));
	}
	if(isCulling)
	{
		while (Working)
		{
			if(LoopCount <= MaxLoops) //Continue looping while verifying floor.
			{
				//Randomly Remove Tiles From Floor.
				TilesCopy = Tiles;
				int ClampedCullingValue = FMath::Clamp(Stream.RandRange(FloorCullingMin,FloorCullingMax) - 1, 0, TilesCopy.Num()/4); // %4 so only a max of a 1/4 will be culled.
				for(int i = 0; i < ClampedCullingValue; i++)
				{
					int RandomIndex = Stream.RandRange(0,TilesCopy.Num() - 1);
					TilesCopy.RemoveAt(RandomIndex);
				}
				ConnectedTiles.Empty();
				ConnectedTiles.Add(TilesCopy[0]);
				while(TileFound)
				{ // Loop through tiles testing if adjacent tiles are present; if so, add to connected Tiles.
					TileFound = false;
					bool isInArray;
					FVector TileLocation;
					for (auto Tile : ConnectedTiles)
					{
						for(int i = 0; i < 3; i++)
						{
							TestRelativeLocation(Tile, TilesCopy,1,0,TileLocation,isInArray);
							if(isInArray)
							{
								ConnectedTiles.Add(TileLocation);
								TilesCopy.Remove(TileLocation);
								TileFound = true;
							}
							TestRelativeLocation(Tile, TilesCopy,0,1,TileLocation,isInArray);
							if(isInArray)
							{
								ConnectedTiles.Add(TileLocation);
								TilesCopy.Remove(TileLocation);
								TileFound = true;
							}
							TestRelativeLocation(Tile, TilesCopy,-1,0,TileLocation,isInArray);
							if(isInArray)
							{
								ConnectedTiles.Add(TileLocation);
								TilesCopy.Remove(TileLocation);
								TileFound = true;
							}
							TestRelativeLocation(Tile, TilesCopy,0,-1,TileLocation,isInArray);
							if(isInArray)
							{
								ConnectedTiles.Add(TileLocation);
								TilesCopy.Remove(TileLocation);
								TileFound = true;
							}
							
						}
					}
					
				}
				if(ConnectedTiles.Num() > RoomSizeMin * RoomSizeMax) //Is connected Tiles Larger than area?:
					{
					Working = false;
					}else
					{ //If its not, try again.
						Working = true;
						++LoopCount;
					}
			}else
			{ //Fail out without culling any tiles.
				Working = false;
				ConnectedTiles = Tiles;
			}
		}
	}else
	{
		ConnectedTiles = Tiles;
	}
	

	for (auto Tile : FloorTilesIn)
	{
		ExtentsX.Add(Tile.X);
		ExtentsY.Add(Tile.Y);
	}
	
	Extents = FVector(ExtentsX.Max(), ExtentsY.Max(), 0);
	LocationOut = RootLocation;
	FloorTilesIn = ConnectedTiles;
	UE_LOG(LogTemp, Warning, TEXT("Returning Floor Values..."));
}

void AGenerator::SpawnTiles()
{
	FVector Location;
	FQuat Rotation;
	bool isFloorF, isFloorR, isFloorD, isFloorL;
	UE_LOG(LogTemp, Warning, TEXT("Preparing Tiles."));
	
	for (auto Tile : FloorTiles)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawning Tiles."));
		FloorISMC->AddInstance(FTransform(Tile * Scale));
		
		for(int i = 0; i < 3; i++)
		{
			TestRelativeLocation(Tile, FloorTiles, 1, 0, Location, isFloorF);
			TestRelativeLocation(Tile, FloorTiles,0, 1, Location, isFloorR);
			TestRelativeLocation(Tile, FloorTiles,-1, 0, Location, isFloorD);
			TestRelativeLocation(Tile, FloorTiles, 0, -1, Location, isFloorL);
			if(!isFloorF)
			{
				Rotation = FQuat::MakeFromEuler(FVector(0,0,90));
				FVector FloorLocation = FVector(Tile)  * Scale;
				FloorLocation.X += Scale;
				WallISMC->AddInstance(FTransform(Rotation, FloorLocation));
			}
			if(!isFloorR)
			{
				Rotation = FQuat::MakeFromEuler(FVector(0,0,0));
				FVector FloorLocation = FVector(Tile)  * Scale;
				FloorLocation.Y += Scale;
				WallISMC->AddInstance(FTransform(Rotation, FloorLocation));
			}
			if(!isFloorD)
			{
				Rotation = FQuat::MakeFromEuler(FVector(0,0,-90));
				FVector FloorLocation = FVector(Tile)  * Scale;
				FloorLocation.Y += Scale;
				WallISMC->AddInstance(FTransform(Rotation, FloorLocation));
			}
			if(!isFloorL)
			{
				Rotation = FQuat::MakeFromEuler(FVector(0,0,180));
				FVector FloorLocation = FVector(Tile)  * Scale;
				FloorLocation.X += Scale;
				WallISMC->AddInstance(FTransform(Rotation, FloorLocation));
			}
		}
	}
}

void AGenerator::FindNextLocation(bool &Valid, FVector &NewLocationtoSpawn)
{
	TArray<int32> Directions;
	bool Searching = true;
	int32 TestIndex;
	for(int i = 0; i < 4; i++)
	{
		Directions.Add(i);
	}
	while (Searching)
	{
		if(Directions.Last() >= 0) //Check for options available.
		{
			TestIndex = Stream.RandRange(0, Directions.Last());
			int32 addedInput;
			FVector Location;
			if(Merging){addedInput = RoomSizeMax;}else{addedInput = RoomSizeMax+1;}
			switch(TestIndex)
			{
			case(0):
				TestRelativeLocation(PreviousLocation, FloorTiles,addedInput,0,Location,isFloorTile);
				break;
			case(1):
				TestRelativeLocation(PreviousLocation, FloorTiles,0,addedInput,Location,isFloorTile);
				break;
			case(2):
				TestRelativeLocation(PreviousLocation, FloorTiles,-addedInput,0,Location,isFloorTile);
				break;
			case(3):
				TestRelativeLocation(PreviousLocation, FloorTiles,0,-addedInput,Location,isFloorTile);
				break;
					
			default: ;
			}
			if(isFloorTile == true)
			{
				Directions.Remove(TestIndex);
				Searching = true;
			}else
			{
				Searching = false;
				ValidTileLocation = true;
				NewLocation = Location;
			}
		}else //Failed to Find New Empty Room Location.
		{
			Searching = false;
			ValidTileLocation = false;
		}
	}
	Valid = ValidTileLocation;
	NewLocationtoSpawn = NewLocation;
}

void AGenerator::CreateCorridors(FVector RoomA, FVector RoomB)
{
	//---------------Get Corners.--------------- 
		int32 aX = RoomA.X;
		int32 aXopp = Rooms.Find(RoomA)->X;
		int32 aY = RoomA.Y;
		int32 aYopp = Rooms.Find(RoomA)->Y;
		int32 aZ = RoomA.Z;
		int32 aZopp = Rooms.Find(RoomA)->Z;
	
		int32 bX = RoomB.X;
		int32 bXopp = Rooms.Find(RoomB)->X;
		int32 bY = RoomB.Y;
		int32 bYopp = Rooms.Find(RoomB)->Y;
		int32 bZ = RoomB.Z;
		int32 bZopp = Rooms.Find(RoomB)->Z;
	/* aX is the first corner of the room,
	 * aXopp is the opposite far corner of the room.
	 * ------------------------------------------*/

	int32 OutputX;
	FVector PointA;
	FVector PointB;

	
	if(FMath::Max(aX, bX) <= FMath::Min(aXopp, bXopp)) //Are rooms parallel to X with overlapping sides.
	{
		if(bY > aY) //Is Room B is to the Right
		{
			if(bY - aYopp > 1) //Are Rooms merged?
			{
				OutputX = Stream.RandRange(FMath::Max(aX, bX), FMath::Min(aXopp, bXopp));//Make Corridor from A to B on Y Axis
				PointA = FVector(OutputX, aYopp, aZ); 
				PointB = FVector(OutputX, bY, bZ);
			}
		}else
		{
			if(aY - bYopp > 1) //Are Rooms merged?
				{
				OutputX = Stream.RandRange(FMath::Max(aX, bX), FMath::Min(aXopp, bXopp));//Make Corridor from B to A on Y Axis
				PointA = FVector(OutputX, aY, aZ); 
				PointB = FVector(OutputX, bYopp, bZ);
				}
		}
	}
	
}


void AGenerator::TestRelativeLocation(FVector ReferenceLocation,TArray<FVector> TestArray, int32 X, int32 Y, FVector &Location, bool &isPresentTile)
{ //Check if Tile relative to Reference has an existing floor tile.
	Location = FVector(ReferenceLocation.X + X,ReferenceLocation.Y + Y,ReferenceLocation.Z);
	isPresentTile = TestArray.Contains(Location);

	//1:13:51
}