// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "mcData.h"
#include "Components/mcBlockComponent.h"
#include "GameFramework/Actor.h"
#include "mcInteractableBlock.h"
#include "Runtime/Core/Public/Async/ParallelFor.h"
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
	TArray<int32> DelayedIndexToSpawn;

	TArray<FIntVector> BlockTransforms;
	TArray<uint8> BlockIDs;

	TArray<int32> DestroyedBlocks;

	TMap<uint8, FBlockDefinition> BlockData;

	FTimerHandle BlockSpawnerTimer = FTimerHandle();
	FTimerHandle GenerationTimer = FTimerHandle();

	bool bFirstLayerGenerated = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool AddBlock(FBlockDefinition BlockData, FIntVector BlockTransform);

	bool LoadChunk();

	UFUNCTION(BlueprintCallable)
	bool RemoveBlock(UmcBlockComponent* BlockReference);

	void LoadChunk_Internal();

	void GenerateNewChunk();

	void GenerateNewChunk_Internal();

	void TickBlockPool();

	void onInteractionFinished(FVector Location, uint8 ChunkBlockID, bool bIsDestroyed);

	AmcInteractableBlock* InteractWithBlock(UInstancedStaticMeshComponent* BlockComponent, int32 Instance);
			
};
