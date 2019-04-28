// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "mcData.h"
#include "Components/mcChunkActor.h"
#include <libnoise.h>
#include "mcWorldManager.generated.h"

USTRUCT()
struct FChunkArray
{
	GENERATED_USTRUCT_BODY()

	TArray<AmcChunkActor*> Y;
};

UCLASS()
class MINECRAFT_CLONE_API AmcWorldManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AmcWorldManager();

	TArray<FChunkArray> ChunkArray;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	noise::module::Perlin PerlinNoise;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Generation")
	TArray<FVector> Vertices;

	UFUNCTION(BlueprintCallable, Category = "World Generation")
	void GenerateWorld(int WorldSize);

	UFUNCTION(BlueprintCallable, Category = "World Generation - Chunks")
	AmcChunkActor* GetChunk(FVector Location);

	UFUNCTION(BlueprintCallable, Category = "World Generation - Chunks")
	AmcChunkActor* SpawnChunk(FVector2D ChunkPosition);

	UFUNCTION(BlueprintCallable, Category = "Block Spawn")
	bool SpawnBlock(FBlockDefinition BlockDefinition, FVector Location);
};
