// Fill out your copyright notice in the Description page of Project Settings.


#include "mcWorldManager.h"

// Sets default values
AmcWorldManager::AmcWorldManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.SetPriorityIncludingPrerequisites(true);
	PrimaryActorTick.TickInterval = 0.016;

}

// Called when the game starts or when spawned
void AmcWorldManager::BeginPlay()
{
	Super::BeginPlay();

	CastedSingleton = Cast<UmcSingleton>(GEngine->GameSingleton);
	
}

// Called every frame
void AmcWorldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetWorld())
	{
		if (LastPlayerLocation != LocationToChunkPos(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation()) || bInitialGeneration)
		{
			LastPlayerLocation = LocationToChunkPos(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation());
			TArray<FIntVector> tempChunks = GetDesiredChunks();

			ChunksToRemove += InactiveChunks;
			InactiveChunks.Empty();

			InactiveChunks.operator+=(ActiveChunks);
			ActiveChunks = tempChunks;

			TArray<FBlockDefinition> tempPossibleBlocks;
			
			for (int x = 0; x < 4; x++)
			{
				tempPossibleBlocks.Add(CastedSingleton->getBlockData(uint8(x)));
			}

			for (int x = 0; x < tempChunks.Num(); x++)
			{
				if (!InactiveChunks.Contains(tempChunks[x]))
				{
					AmcChunkActor* tempChunk = GetChunk(tempChunks[x]);

					if (!tempChunk->LoadChunk())
					{
						GenerateWorld(ChunkSize.X, FIntVector(tempChunk->GetActorLocation()), 1234);

						for (FVector Vertex : Vertices)
						{
							tempChunk->AddBlock(tempPossibleBlocks[(Vertex.Z > 100 ? 0 : (Vertex.Z > -500 ? 3 : 2))], FIntVector(Vertex - tempChunk->GetActorLocation()));
						}

						tempChunk->GenerateNewChunk();
					}
					
				}
				else
				{
					InactiveChunks.Remove(tempChunks[x]);
				}
			}

			if (bInitialGeneration)
				bInitialGeneration = false;

		}
		if (ChunksToRemove.IsValidIndex(0))
		{
			ChunkMap[ChunksToRemove.Last()]->Destroy();
			ChunkMap.Remove(ChunksToRemove.Pop());
		}

	}

}

void AmcWorldManager::GenerateWorld(int WorldSize, FIntVector Offset, int32 Seed)
{
	float NoiseInputScale = 0.01; // Making this smaller will "stretch" the perlin noise terrain
	float NoiseOutputScale = 2200; // Making this bigger will scale the terrain's height


	Offset.X -= WorldSize * 0.5*BlockSize;
	Offset.Y -= WorldSize * 0.5*BlockSize;

	PerlinNoise.SetSeed((int)Seed);

	Vertices.Init(FVector(0, 0, 0), WorldSize * WorldSize);
	for (int y = 0; y < WorldSize; y++) {
		for (int x = 0; x < WorldSize; x++) {
			float NoiseResult = PerlinNoise.GetValue(float((x+(Offset.X/BlockSize)) * NoiseInputScale), float((y+(Offset.Y / BlockSize)) * NoiseInputScale), 1.0);
			
			//flatten and smooth land bellow sealevel

			NoiseResult = ProcessNoiseValue(NoiseResult);

			int index = x + y * WorldSize;
			FVector2D Position = FVector2D(x * BlockSize,y * BlockSize);
			Vertices[index] = FVector(Position.X+Offset.X, Position.Y+Offset.Y, SnapToGrid(NoiseResult * NoiseOutputScale));
		}
	}
}

float AmcWorldManager::ProcessNoiseValue(float NoiseValue)
{
	if (NoiseValue < 0)
	{
		NoiseValue = -FMath::Pow(abs(NoiseValue*.8), 2);
	}
	return NoiseValue;
}

AmcChunkActor* AmcWorldManager::GetChunk(FIntVector Location)
{
	if (ChunkMap.Contains(Location))
	{
		AmcChunkActor* tempChunk = ChunkMap[Location];
		if(IsValid(tempChunk))
			return tempChunk;

		tempChunk = SpawnChunk(Location);
		ChunkMap[Location] = tempChunk;
		return tempChunk;

	}
	else
	{
		AmcChunkActor* tempChunk = SpawnChunk(Location);
		ChunkMap.Add(Location, tempChunk);
		return tempChunk;
	}

	return NULL;
}

AmcChunkActor* AmcWorldManager::SpawnChunk(FIntVector ChunkLocation)
{
	if (GetWorld())
	{
		return GetWorld()->SpawnActor<AmcChunkActor>(AmcChunkActor::StaticClass(), FVector(FIntVector(ChunkLocation.X*ChunkSize.X, ChunkLocation.Y*ChunkSize.Y, ChunkLocation.Z*ChunkSize.Z).operator*(BlockSize)), FRotator(0,0,0), FActorSpawnParameters());
	}
	return NULL;
}

bool AmcWorldManager::SpawnBlock(FBlockDefinition BlockData, FVector Location)
{
	AmcChunkActor* tempChunk = GetChunk(LocationToChunkPos(Location));
	if (tempChunk)
		return tempChunk->AddBlock(BlockData, FIntVector(Location - tempChunk->GetActorLocation()) / BlockSize);
	return false;
}

FIntVector AmcWorldManager::ChunkToLocal(FIntVector ChunkPos)
{
	int32 Offset = (GenerationDistance - 1) / 2;
	return FIntVector(ChunkPos.X - LastPlayerLocation.X+ Offset, ChunkPos.Y - LastPlayerLocation.Y+ Offset, 0);
}

TArray<FIntVector> AmcWorldManager::GetDesiredChunks()
{
	FIntVector OriginChunk = LocationToChunkPos(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation());
	
	TArray<FIntVector> resultArray;
	int32 Offset = (GenerationDistance - 1) / 2;

	for (int X = 0; X < GenerationDistance; X++)
	{
		for (int Y = 0; Y < GenerationDistance; Y++)
		{
			resultArray.AddUnique(FIntVector(OriginChunk.X + X - Offset, OriginChunk.Y + Y - Offset, OriginChunk.Z));
		}
	}

	return resultArray;
}

