// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "mcData.h"
#include "Components/mcBlockComponent.h"
#include "GameFramework/Actor.h"
#include "mcInteractableBlock.h"
#include "mcChunkSave.h"
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
	TMap<int32, UmcBlockComponent*> BlockPlayerComponents;


	//unreal arrays get exponentially more expensisive the more items they have (for eg adding new item to an array caused the old array to copy over to a newly created larger array)
	// splitting the large arrays into multiple smaller one yields significant performance benefits

	//index represents block component ID
	UPROPERTY()
	TArray<FChunkSectionData> BlockComponentData;
	UPROPERTY()
	TArray<FChunkSectionData> BlockComponentPlayerData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Data)
	TArray<int32> BlockComponentToSpawn;

	UPROPERTY()
	TMap<uint8, FBlockDefinition> BlockData;

	FTimerHandle BlockSpawnerTimer = FTimerHandle();
	FTimerHandle GenerationTimer = FTimerHandle();

	bool bInitialLoad = true;

	bool bFirstLayerGenerated = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool AddBlock(FBlockDefinition localBlockData, FIntVector BlockTransform, bool bInstantAdd = false);

	bool LoadChunk(bool bTemporary = true);

	UFUNCTION(BlueprintCallable)
	bool RemoveBlock(UmcBlockComponent* BlockReference);

	void LoadChunk_Internal(bool bTemporary = true);

	void SaveChunk();

	void GenerateNewChunk();

	void GenerateNewChunk_Internal();

	void TickBlockPool();

	UFUNCTION()
	void onInteractionFinished(FVector Location, uint8 ChunkBlockID, bool bIsDestroyed);

	UFUNCTION(BlueprintCallable)
	AmcInteractableBlock* InteractWithBlock(UInstancedStaticMeshComponent* BlockComponent, int32 Instance);

	UmcBlockComponent* GetBlockComponent(int32 ComponentIndex, bool bPlayerModification = false);
			
};
