// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "mcData.h"
#include "Components/mcBlockComponent.h"
#include "GameFramework/Actor.h"
#include "mcChunkActor.generated.h"

//forward declare class (preventing possible cyclic dependancy issues)
//required class header is then included in the cpp
class UmcBlockComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINECRAFT_CLONE_API AmcChunkActor : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AmcChunkActor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	TMap<int32, UmcBlockComponent*> BlockComponents;

	TArray<int32> BlockIndexToSpawn;

	TArray<FIntVector> BlockTransforms;
	TArray<uint8> BlockIDs;

	TMap<uint8, FBlockDefinition> BlockData;

	FTimerHandle BlockSpawnerTimer = FTimerHandle();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool AddBlock(FBlockDefinition BlockData, FIntVector BlockTransform);

	void LoadChunk();

	UFUNCTION(BlueprintCallable)
	bool RemoveBlock(UmcBlockComponent* BlockReference);

	void TickBlockPool();
			
};
