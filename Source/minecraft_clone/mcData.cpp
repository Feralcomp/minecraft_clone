// Fill out your copyright notice in the Description page of Project Settings.

#include "mcData.h"
#include "EngineMinimal.h"

FVector SimpleTransformToVector(FByteTransform Transform)
{
	return FVector(Transform.PosX, Transform.PosY, Transform.PosZ)*BlockSize;
}

FIntVector LocationToChunkPos(FVector InLocation)
{
	return FIntVector( FMath::FloorToInt(InLocation.X / (ChunkSize.X*BlockSize)), FMath::FloorToInt(InLocation.Y / (ChunkSize.Y*BlockSize)), InLocation.Z = 0 ? 1 : 0);
}

int32 SnapToGrid(float value)
{
	return float(FMath::RoundToInt(value / BlockSize)*BlockSize);
}

FVector SnapVectorToGrid(FVector Value)
{
	return(FVector(SnapToGrid(Value.X), SnapToGrid(Value.Y), SnapToGrid(Value.Z)));
}