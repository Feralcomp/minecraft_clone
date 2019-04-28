// Fill out your copyright notice in the Description page of Project Settings.


#include "mcWorldManager.h"

// Sets default values
AmcWorldManager::AmcWorldManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AmcWorldManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AmcWorldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AmcWorldManager::GenerateWorld(int WorldSize)
{
	float NoiseInputScale = 0.01; // Making this smaller will "stretch" the perlin noise terrain
	float NoiseOutputScale = 1600; // Making this bigger will scale the terrain's height

	Vertices.Init(FVector(0, 0, 0), WorldSize * WorldSize);
	for (int y = 0; y < WorldSize; y++) {
		for (int x = 0; x < WorldSize; x++) {
			float NoiseResult = PerlinNoise.GetValue(float(x * NoiseInputScale), float(y * NoiseInputScale), 1.0) * NoiseOutputScale;
			int index = x + y * WorldSize;
			FVector2D Position = FVector2D(x * BlockSize,y * BlockSize);
			Vertices[index] = FVector(Position.X, Position.Y, SnapToGrid(NoiseResult));
		}
	}
}

AmcChunkActor* AmcWorldManager::GetChunk(FVector Location)
{
	FIntVector ChunkPosition = LocationToChunkPos(Location);
	if (ChunkPosition.Z == 0 && ChunkPosition.X >= 0 && ChunkPosition.Y >= 0)
	{
		if (!ChunkArray.IsValidIndex(ChunkPosition.X))
		{
			TArray<AmcChunkActor*> tempArray;
			FChunkArray tempChunkElement;

			tempArray.Init(NULL, ChunkPosition.Y + 1);
			tempChunkElement.Y = tempArray;

			ChunkArray.Add(tempChunkElement);
		}

		if (ChunkArray[ChunkPosition.X].Y.IsValidIndex(ChunkPosition.Y))
		{
			if (IsValid(ChunkArray[ChunkPosition.X].Y[ChunkPosition.Y]))
			{
				if(ChunkArray[ChunkPosition.X].Y[ChunkPosition.Y] != NULL)
				return ChunkArray[ChunkPosition.X].Y[ChunkPosition.Y];
			}

			ChunkArray[ChunkPosition.X].Y.Add(SpawnChunk(FVector2D(ChunkPosition.X, ChunkPosition.Y)));
			return ChunkArray[ChunkPosition.X].Y[ChunkPosition.Y];
		}

	}
	return NULL;
}

AmcChunkActor* AmcWorldManager::SpawnChunk(FVector2D ChunkPosition)
{
	if (GetWorld())
	{
		return GetWorld()->SpawnActor<AmcChunkActor>(AmcChunkActor::StaticClass(), FVector(ChunkPosition.X * ChunkSize.X, ChunkPosition.Y * ChunkSize.Y, 0), FRotator(), FActorSpawnParameters());
	}
	return NULL;
}

bool AmcWorldManager::SpawnBlock(FBlockDefinition BlockData, FVector Location)
{
	AmcChunkActor* tempChunk = GetChunk(Location);
	if (IsValid(tempChunk))
	{
		return tempChunk->AddBlock(BlockData, FIntVector(Location - tempChunk->GetActorLocation()));
	}
	return false;
}