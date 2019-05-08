// Fill out your copyright notice in the Description page of Project Settings.


#include "mcWorldManager.h"

// Sets default values
AmcWorldManager::AmcWorldManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.SetPriorityIncludingPrerequisites(true);
	PrimaryActorTick.TickInterval = 0.016;

}

// Called when the game starts or when spawned
void AmcWorldManager::BeginPlay()
{
	Super::BeginPlay();

	CastedSingleton = Cast<UmcSingleton>(GEngine->GameSingleton);
	if (IsValid(GetWorld()) && GetWorld()!=NULL)
	{
		UmcGameInstance * tempGameInstance = Cast<UmcGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (IsValid(tempGameInstance) && tempGameInstance->bShouldLoadSave)
		{
			UmcSaveGame* LoadGameInstance = Cast<UmcSaveGame>(UGameplayStatics::LoadGameFromSlot("Map1", 0));
			WorldSeed = LoadGameInstance->WorldSeed;
			bGameLoaded = true;
		}
	}

	FString tempPath = FPaths:: GameSavedDir() + "SaveGames/Temporary/";
	FPlatformFileManager::Get().GetPlatformFile().DeleteDirectoryRecursively(*tempPath);

	PrimaryActorTick.SetTickFunctionEnable(true);
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

			for (FIntVector ChunkPos : tempChunks)
			{
				if (!InactiveChunks.Contains(ChunkPos))
				{
					AmcChunkActor* tempChunk = GetChunk(ChunkPos);

					if (!tempChunk->LoadChunk(!bGameLoaded))
					{
						TArray<FVector> tempVertices = GenerateWorld(ChunkSize.X, FIntVector(tempChunk->GetActorLocation()), WorldSeed);

						ParallelFor(tempVertices.Num(), [&](int32 x)
						{
							FIntVector tempPos = FIntVector(tempVertices[x] - tempChunk->GetActorLocation());
							FBlockDefinition tempBlockData = tempPossibleBlocks[(tempVertices[x].Z > 100 ? 0 : (tempVertices[x].Z > -500 ? 3 : 2))];
							
							ThreadMutex.Lock();
							tempChunk->AddBlock(tempBlockData,tempPos);
							ThreadMutex.Unlock();
						});

						tempChunk->GenerateNewChunk();
					}

					if (ChunksToRemove.Contains(ChunkPos))
					{
						ChunksToRemove.Remove(ChunkPos);
					}
					
				}
				else
				{
					InactiveChunks.Remove(ChunkPos);
				}
			}

			if (bInitialGeneration)
				bInitialGeneration = false;

		}
		if (ChunksToRemove.IsValidIndex(0) && ChunkMap.Contains(ChunksToRemove.Last()))
		{
			ChunkMap[ChunksToRemove.Last()]->SaveChunk();
			ChunkMap[ChunksToRemove.Last()]->Destroy();
			ChunkMap.Remove(ChunksToRemove.Pop());
		}

	}

}

TArray<FVector> AmcWorldManager::GenerateWorld(int WorldSize, FIntVector Offset, int32 Seed)
{
	float NoiseInputScale = 0.01; // Making this smaller will "stretch" the perlin noise terrain
	float NoiseOutputScale = 2200; // Making this bigger will scale the terrain's height

	TArray<FVector> GeneratedVertices;

	Offset.X -= WorldSize * 0.5*BlockSize;
	Offset.Y -= WorldSize * 0.5*BlockSize;

	PerlinNoise.SetSeed((int)Seed);

	GeneratedVertices.Init(FVector(0, 0, 0), WorldSize * WorldSize);

	ParallelFor (WorldSize, [&](int y) {
		for (int x = 0; x < WorldSize; x++) {
			float NoiseResult = PerlinNoise.GetValue(float((x+(Offset.X/BlockSize)) * NoiseInputScale), float((y+(Offset.Y / BlockSize)) * NoiseInputScale), 1.0);
			
			//flatten and smooth land bellow sealevel

			NoiseResult = ProcessNoiseValue(NoiseResult);

			int index = x + y * WorldSize;
			FVector2D Position = FVector2D(x * BlockSize,y * BlockSize);

			ThreadMutex.Lock();
			GeneratedVertices[index] = FVector(Position.X+Offset.X, Position.Y+Offset.Y, SnapToGrid(NoiseResult * NoiseOutputScale));
			ThreadMutex.Unlock();
		}
	});
	return GeneratedVertices;
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

