// Fill out your copyright notice in the Description page of Project Settings.

#include "mcData.h"
#include "minecraft_clone.h"
#include "Kismet/GameplayStatics.h"
#include "EngineMinimal.h"

FVector SimpleTransformToVector(FByteTransform Transform)
{
	return FVector(Transform.PosX, Transform.PosY, Transform.PosZ)*BlockSize;
}

FIntVector LocationToChunkPos(FVector InLocation)
{
	return FIntVector( FMath::FloorToInt(InLocation.X / ChunkSize.X), FMath::FloorToInt(InLocation.Y / ChunkSize.Y), InLocation.Z = 0 ? 1 : 0);
}

int32 SnapToGrid(float value)
{
	return float(int(value / BlockSize)*BlockSize);
}