// Fill out your copyright notice in the Description page of Project Settings.


#include "PCGTesting/public/Door.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "PCGTesting/PCGTestingCharacter.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("DoorFrame");

	DoorCollision = CreateDefaultSubobject<UBoxComponent>("Collision");
	DoorCollision->SetupAttachment(DoorMesh);

	Arrow = CreateDefaultSubobject<UArrowComponent>("Arrow");
	Arrow->SetupAttachment(DoorMesh);
	
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	DoorCollision->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OnBoxOverlap);
}

void ADoor::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult)
{
	APCGTestingCharacter* Player = Cast<APCGTestingCharacter>(OtherActor);
	if(Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("New Room"));
	}
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

