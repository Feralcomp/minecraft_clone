// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "mcSingleton.h"
#include "mcData.h"
#include "Components/mcChunkActor.h"
#include <libnoise.h>
#include "Runtime/Core/Public/Async/ParallelFor.h"
#include "mcWorldManager.generated.h"

USTRUCT()
struct FChunkArray
{
	GENERATED_USTRUCT_BODY()

		TArray<AmcChunkActor*> Y;
	TArray<AmcChunkActor*> negY;
};


UCLASS()
class MINECRAFT_CLONE_API AmcWorldManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AmcWorldManager();

	TMap<FIntVector, AmcChunkActor*> ChunkMap;

	TArray<FBlockData> BlockPool;

	FTimerHandle BlockSpawnerTimer = FTimerHandle();

	FIntVector LastPlayerLocation;

	UmcSingleton* CastedSingleton;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	noise::module::Perlin PerlinNoise;

	float ProcessNoiseValue(float NoiseInputValue);

	FCriticalSection ThreadMutex;

public:	
	UPROPERTY(BlueprintReadOnly, Category = "World Generation")
	TArray<FVector> Vertices;

	TArray<FIntVector> ActiveChunks;
	TArray<FIntVector> InactiveChunks;
	TArray<FIntVector> ChunksToRemove;

	UPROPERTY(BlueprintReadOnly, Category = "World Generation")
		int32 WorldSeed = FMath::Rand();

	bool bInitialGeneration = true;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "World Generation")
	TArray<FVector> GenerateWorld(int WorldSize, FIntVector Offset, int32 Seed);

	UFUNCTION(BlueprintCallable, Category = "World Generation - Chunks")
	AmcChunkActor* GetChunk(FIntVector Location);

	UFUNCTION(BlueprintCallable, Category = "World Generation - Chunks")
	AmcChunkActor* SpawnChunk(FIntVector Location);

	UFUNCTION(BlueprintCallable, Category = "Block Spawn")
	bool SpawnBlock(FBlockDefinition BlockDefinition, FVector Location);

	UFUNCTION(BlueprintCallable, Category = "Chunks")
	TArray<FIntVector> GetDesiredChunks();

	UFUNCTION(BlueprintCallable, Category = "Chunks")
	FIntVector ChunkToLocal(FIntVector ChunkPos);
};
