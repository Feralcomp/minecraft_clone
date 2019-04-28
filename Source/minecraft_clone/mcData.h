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

static int32 BlockSize = 100;

static FIntVector ChunkSize = FIntVector(8, 8, 32);

FVector SimpleTransformToVector(FByteTransform Transform);

int32 SnapToGrid(float value);

FIntVector LocationToChunkPos(FVector InLocation);