// Fill out your copyright notice in the Description page of Project Settings.


#include "Engine/DataTable.h"
#include "mcData.generated.h"

#pragma once

/**
 * defining data structures used within the project
 */



 //struct to hold texture data per block (bottom/side/top)
USTRUCT(BlueprintType)
struct FBlockTexture
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlockTextureData")
		UTexture* Side;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlockTextureData")
		UTexture* Top;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlockTextureData")
		UTexture* Bottom;
};

/** Structure that defines a table entry for blocks */
USTRUCT(BlueprintType)
struct FBlockDefinition : public FTableRowBase //using table to store cube definitions so that more cubes can be defined later
{
	GENERATED_USTRUCT_BODY();

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Data")
		uint8 ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Data")
		FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Data")
		uint8 Hardness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Data")
		FBlockTexture BlockTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Data")
		UStaticMesh* BlockMesh;

};

//optimised transform struct, used for world saving (using vectors to store block locations is more costly)
USTRUCT(BlueprintType)
struct FByteTransform
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Data") 
	uint8 PosX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Data")
	uint8 PosY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Data")
	uint8 PosZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Data")
	uint8 Rotation;

};

USTRUCT()
struct FChunkData
{
	GENERATED_USTRUCT_BODY()

	FVector Location;
	TArray<FBlockDefinition> BlockData;
	TArray<FVector> BlockLocations;
	FChunkData()
	{
		Location = FVector(0);
		BlockData.Empty();
		BlockLocations.Empty();
	}

};

USTRUCT()
struct FBlockData
{

	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FBlockDefinition Data;

	UPROPERTY()
	FVector Location;

};

USTRUCT()
struct FChunkSectionData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<int32> BlockIndexToSpawn;
	
	UPROPERTY()
	TArray<int32> DelayedIndexToSpawn;

	UPROPERTY()
	TArray<FIntVector> BlockTransforms;

	UPROPERTY()
	TArray<int32> DestroyedBlocks;

	FChunkSectionData()
	{
		BlockTransforms.Reserve(4096);
		BlockIndexToSpawn.Reset(1);
		DelayedIndexToSpawn.Reset(1);
		DestroyedBlocks.Reset(1);
	}
};

static int32 BlockSize = 100;

static int32 GenerationDistance = 9; //in number of chunks (so if 5, we spawn 5x5 chunks around player)

static FIntVector ChunkSize = FIntVector(16, 16, 64);

FVector SimpleTransformToVector(FByteTransform Transform);

int32 SnapToGrid(float value);
FVector SnapVectorToGrid(FVector Value);

FIntVector LocationToChunkPos(FVector InLocation);