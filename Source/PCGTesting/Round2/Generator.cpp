// Fill out your copyright notice in the Description page of Project Settings.


#include "PCGTesting/Round2/Generator.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
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
	Rooms.Empty();
	CorridorTiles.Empty();
	FloorTiles.Empty();
	
	if(NewSeed == true) //On click randomise and set new seed;
	{
		NewSeed = false;
		Seed = rand () % 90000; 
		Stream.Initialize(Seed); //This Stream will allow for seeds to be re-created
	}
	Stream.Initialize(Seed); //Manually update seed;
	GenerateMap(); //Begin making the dungeons
}

void AGenerator::GenerateMap()
{
	FVector NextLocation;
	FVector LocationOut;
	FVector Extents;
	TArray<FVector> FloorTilesIn;
	TArray<FVector> CorridorTilesIn;
	UE_LOG(LogTemp, Warning, TEXT("Generating Map..."));
	for(int i = 0; i < RoomCount; i++)
	{ 
		if(i == 0) //Generates first location of Actor 0,0,0.
		{
			MakeFloor(PreviousLocation, FloorTilesIn, LocationOut, Extents);
			FloorTiles = FloorTilesIn;
			Rooms.Add(LocationOut, Extents);
			UE_LOG(LogTemp, Warning, TEXT("First Room Added"));
		}else
		{
			bool Valid;
			FindNextLocation(Valid, NextLocation);
			if(Valid == true) //If next location is a suitable location then continue to spawn next room. 
			{
				MakeFloor(NextLocation, FloorTilesIn, LocationOut, Extents);
				FloorTiles.Append(FloorTilesIn); //Adds the successful floor tiles to spawn.
				//Using the extents of the room which is the further point and the origin point, adds to TMap of Rooms. 
				Rooms.Add(LocationOut, Extents); 
				UE_LOG(LogTemp, Warning, TEXT("Another Room Added"));
				
				CreateCorridors(PreviousLocation, NextLocation, CorridorTilesIn); //Make Corridor Between Last 2 Rooms.
				CorridorTiles.Append(CorridorTilesIn); 
				

				PreviousLocation = NextLocation; //Update location for next room.
			}
		}
	}
	SpawnTiles(); 
}

void AGenerator::MakeFloor(FVector Location, TArray<FVector> &FloorTilesIn, FVector &LocationOut, FVector &Extents) 
{
	//Get Random X and Y Lengths & Make array of Floor Tiles from Root Position.
	FVector RootLocation = Location;

	//Output of Length + Width of the Room depended on Stream
	int32 OutX = Stream.RandRange(RoomSizeMin, RoomSizeMax); 
	int32 OutY = Stream.RandRange(RoomSizeMin, RoomSizeMax);
	//Based on Length + Width Area is calculated
	int32 Area = (OutX * OutY) - 1;
	
	Working = true;
	TArray<FVector> Tiles;
	TArray<int32> ExtentsX;
	TArray<int32> ExtentsY;
	int32 MaxExtentsX;
	int32 MaxExtentsY;
	TArray<FVector> ConnectedTiles;
	TArray<FVector> TilesCopy;
	bool TileFound = true;
	int32 LoopCount = 0;
	
	UE_LOG(LogTemp, Warning, TEXT("Adding Floor Tiles To Array"));
	for(int i = 0; i <= Area; i++) 
	{ //Adds a Tile for each space in the area defined by Min and Max of room size, with the root location being the origin displacement.
		Tiles.Add(FVector((i / OutY) + RootLocation.X, (i % OutY) + RootLocation.Y, RootLocation.Z));
	}  //Adds floors to spawn to Tiles array, in which this array will all be spawned in another function. 
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
				{ //Loop through this dependent on the clamped value which calculates a max of a 1/4 of the tiles to remove. 
					int RandomIndex = Stream.RandRange(0,TilesCopy.Num() - 1); //This will provide a random index of the tiles in which to remove. 
					TilesCopy.RemoveAt(RandomIndex);
				}
				ConnectedTiles.Empty();
				ConnectedTiles.Add(TilesCopy[0]);
				while(TileFound)
				{ // Loop through tiles testing if adjacent tiles are present; if so, add to connected Tiles.
					TileFound = false;
					bool isInArray;
					FVector TileLocation;
					for (auto Tile : ConnectedTiles) //Loop through each tile in ConnectedTiles Array. 
					{
						for(int i = 0; i < 3; i++)  //Checks all directions. 
						{
							TestRelativeLocation(Tile, TilesCopy,1,0,TileLocation,isInArray);
							if(isInArray)
							{
								ConnectedTiles.Add(TileLocation); //If there is a tile next to it, success
								TilesCopy.Remove(TileLocation); //and so remove that tile from the tiles array to be culled.
								TileFound = true; 
							}
							//Repeat for all directions based on 1 up, 1 right, 1 left, 1 down from that tile...
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
			}else //Safety net so rooms don't spawn with dead space rooms with no entry/exit.
			{ //If there are no tiles around the tile,
				//then the culling will not proceed and the room will spawn normal with no removals.
				Working = false;
				ConnectedTiles = Tiles; //Set back to original tiles - not the copied tiles with removed tiles. 
			}
		}
	}else
	{
		ConnectedTiles = Tiles;
	}
	

	for (auto Tile : ConnectedTiles)
	{
		ExtentsX.Add(Tile.X);
		ExtentsY.Add(Tile.Y);
	}
	int32 Index_X;
	int32 Index_Y;
	UKismetMathLibrary::MaxOfIntArray(ExtentsY, Index_Y, MaxExtentsY);
	UKismetMathLibrary::MaxOfIntArray(ExtentsX, Index_X, MaxExtentsX);
	
	Extents = FVector(MaxExtentsX, MaxExtentsY, RootLocation.Z);
	//Extents = FVector(ExtentsX.Max, ExtentsY.Max(), 0);  I hate this, this line of code killed about 5 hours of my life
	LocationOut = RootLocation;
	FloorTilesIn = ConnectedTiles;
}

void AGenerator::SpawnTiles()
{
	FVector Location;
	FQuat Rotation;
	bool isFloorF, isFloorR, isFloorD, isFloorL;
	UE_LOG(LogTemp, Warning, TEXT("Preparing To Spawn Tiles."));
	
	FloorTiles.Append(CorridorTiles);
	
	for (auto Tile : FloorTiles)
	{
		FloorISMC->AddInstance(FTransform(Tile * Scale)); // Spawning Floor.
		FVector RoofLocation = Tile * Scale; //Upshifting location to be at the top of the walls by 'Scale'.
		RoofLocation.Z += Scale;
		FloorISMC->AddInstance(FTransform(RoofLocation)); // Spawning Roof.
		
		for(int i = 0; i < 3; i++) //Loop for each direction. 
		{
			//Check each direction if there is a tile or empty space. If empty space, means end of the room, so spawn door. 
			TestRelativeLocation(Tile, FloorTiles, 1, 0, Location, isFloorF);
			TestRelativeLocation(Tile, FloorTiles,0, 1, Location, isFloorR); 
			TestRelativeLocation(Tile, FloorTiles,-1, 0, Location, isFloorD);
			TestRelativeLocation(Tile, FloorTiles, 0, -1, Location, isFloorL);
			if(!isFloorF) //Is there NOT a floor Forward?
			{ //Spawn Wall
				Rotation = FQuat::MakeFromEuler(FVector(0,0,90));
				FVector FloorLocation = FVector(Tile)  * Scale; 
				FloorLocation.X += Scale; //Assure the length of each wall is added to the location.
				//Using Instanced Static Mesh Components to add instances in the world. 
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
			if(Merging){addedInput = RoomSizeMax;}else{addedInput = RoomSizeMax+1;} //Sets the added input to the Maximum room size
			switch(TestIndex)  
			{
			case(0): //if merging is enabled, the rooms will check for spaces direction next to the spawned room as well as spaces apart from it. 
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

void AGenerator::CreateCorridors(FVector RoomA, FVector RoomB, TArray<FVector> &CorridorTilesReturn)
{
	//---------------Get Corners.--------------- 
	int32 aX = RoomA.X;
	int32 aXopp = Rooms.FindRef(RoomA).X;
	int32 aY = RoomA.Y;
	int32 aYopp = Rooms.FindRef(RoomA).Y;
	int32 aZ = RoomA.Z;
	int32 aZopp = Rooms.FindRef(RoomA).Z;
	
	int32 bX = RoomB.X;
	int32 bXopp = Rooms.FindRef(RoomB).X;
	int32 bY = RoomB.Y;
	int32 bYopp = Rooms.FindRef(RoomB).Y;
	int32 bZ = RoomB.Z;
	int32 bZopp = Rooms.FindRef(RoomB).Z;
	/* aX is the first corner of the room,
	 * aXopp is the opposite far corner of the room.
	 * ------------------------------------------*/

	int32 OutputX;
	int32 OutputY;
	FVector PointA;
	FVector PointB;
	TArray<FVector> CorridorTilesIn;

	UE_LOG(LogTemp, Warning, TEXT("Creating a Corridor."));
	
	if(FMath::Max(aX, bX) <= FMath::Min(aXopp, bXopp)) //Are rooms parallel to X with overlapping sides.
	{
		if(bY > aY) //Is Room B is to the Right
		{
			UE_LOG(LogTemp, Warning, TEXT("Room is to the Right"));
			if(bY - aYopp > 1) //Are Rooms merged?
			{
				OutputX = Stream.RandRange(FMath::Max(aX, bX), FMath::Min(aXopp, bXopp));//Make Corridor from A to B on Y Axis
				PointA = FVector(OutputX, aYopp, aZ); 
				PointB = FVector(OutputX, bY, bZ);
				SpawnCorridorsY(PointA, PointB, CorridorTilesIn);
			}
		}else if(aY > bY)//B is to the left
		{
			UE_LOG(LogTemp, Warning, TEXT("Room is to the Left"));
			if(aY - bYopp > 1) //Are Rooms merged?
			{
				UE_LOG(LogTemp, Warning, TEXT("Room is merged."));
				OutputX = Stream.RandRange(FMath::Max(aX, bX), FMath::Min(aXopp, bXopp));//Make Corridor from B to A on Y Axis
				PointA = FVector(OutputX, aY, aZ); 
				PointB = FVector(OutputX, bYopp, bZ);
				SpawnCorridorsY(PointB, PointA, CorridorTilesIn);
			}
		}
	}else //if(FMath::Max(aY, bY) <= FMath::Min(aYopp, bYopp)) //Are rooms parallel to Y with overlapping sides.
	{
		
		if(bX > aX) //Is Room B is forward
		{
			UE_LOG(LogTemp, Warning, TEXT("Room is to the Forward"));
			if(bX - aXopp > 1) //Are Rooms merged?
			{
				OutputY = Stream.RandRange(FMath::Max(aY, bY), FMath::Min(aYopp, bYopp));//Make Corridor from A to B on X Axis
				PointA = FVector(aXopp, OutputY, aZ); 
				PointB = FVector(bX, OutputY, bZ);
				SpawnCorridorsX(PointA, PointB, CorridorTilesIn);
			}
		}else if(aX > bX)//A is Forward
		{
			UE_LOG(LogTemp, Warning, TEXT("Room is to the Backwards"));
			if(aX - bXopp > 1) //Are Rooms merged?
			{
				UE_LOG(LogTemp, Warning, TEXT("Room is merged."));
				OutputY = Stream.RandRange(FMath::Max(aY, bY), FMath::Min(aYopp, bYopp));//Make Corridor from B to A on X Axis
				PointA = FVector(aX, OutputY, aZ); 
				PointB = FVector(bXopp, OutputY, bZ);
				SpawnCorridorsX(PointB, PointA, CorridorTilesIn);
			}
		}
	}

	CorridorTilesReturn = CorridorTilesIn;
}

void AGenerator::SpawnCorridorsY(FVector Start, FVector Finish, TArray<FVector> &FloorTilesReturn)
{
	TArray<FVector> Tiles;
	
	for(int i = 0; i < FMath::Abs(Start.Y - Finish.Y); i++)
	//for (int i = FMath::Min(Start.Y, Finish.Y) + 1; i < FMath::Max(Start.Y, Finish.Y); i++)
	{
		Tiles.Add(FVector(Start.X, Start.Y + i, Start.Z));
		UE_LOG(LogTemp, Warning, TEXT("Added Corridor on Y axis"))
	}
	
	FloorTilesReturn.Append(Tiles);
}

void AGenerator::SpawnCorridorsX(FVector Start, FVector Finish, TArray<FVector> &FloorTilesReturn)
{
	TArray<FVector> Tiles;
	
	for(int i = 0; i < FMath::Abs(Start.X - Finish.X); i++)
	//for (int i = FMath::Min(Start.X, Finish.X) + 1; i < FMath::Max(Start.X, Finish.X); i++)
	{
		Tiles.Add(FVector(Start.X + i, Start.Y, Start.Z));
		UE_LOG(LogTemp, Warning, TEXT("Added Corridor on X axis"))
	}
	
	FloorTilesReturn.Append(Tiles);
}

void AGenerator::TestRelativeLocation(FVector ReferenceLocation,TArray<FVector> TestArray, int32 X, int32 Y, FVector &Location, bool &isPresentTile)
{ //Check if Tile relative to Reference has an existing floor tile.
	Location = FVector(ReferenceLocation.X + X,ReferenceLocation.Y + Y,ReferenceLocation.Z);
	isPresentTile = TestArray.Contains(Location);
}
