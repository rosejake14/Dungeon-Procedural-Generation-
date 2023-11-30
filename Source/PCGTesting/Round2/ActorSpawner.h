// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActorSpawner.generated.h"

class AGenerator;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PCGTESTING_API UActorSpawner : public UActorComponent
{
	GENERATED_BODY()

public:	
	UActorSpawner();
	void SpawnItems();
	void CreateInstances();
	bool RandomBoolWithWeightFromStream(float Weight);

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
	int32 Quantity;
	UPROPERTY(EditAnywhere)
	bool RoomSpawnOnly;
	
	UPROPERTY(EditAnywhere)
	TMap<TSubclassOf<AActor>, float> SpawnList;

	TArray<FVector>FloorTilesCopy;
	AGenerator* GeneratorRef;
	UPROPERTY(EditAnywhere)
	float MinDistance = 0.0001;

	UPROPERTY(EditAnywhere)
	FVector Offset;
};
