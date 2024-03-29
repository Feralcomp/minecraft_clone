// Fill out your copyright notice in the Description page of Project Settings.


#include "mcChunkActor.h"

// Sets default values for this component's properties
AmcChunkActor::AmcChunkActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.2;

	//ensure arras can hold at least 4 cube types to reduce resizing (we only have 4 types of cubes atm anyway)
	BlockComponentData.Init(FChunkSectionData() ,4);
	BlockComponentPlayerData.Init(FChunkSectionData(), 4);
	// ...
}


// Called when the game starts
void AmcChunkActor::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void AmcChunkActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if spawner isnt ticking and there is something to spawn
	if (!GetWorld()->GetTimerManager().IsTimerActive(BlockSpawnerTimer))
	{
		if(BlockComponentToSpawn.IsValidIndex(0))
			GetWorld()->GetTimerManager().SetTimer(BlockSpawnerTimer, this, &AmcChunkActor::TickBlockPool, float(1.0 / (bFirstLayerGenerated ? 384 : 768)), true, FMath::FRand()*.5);
	}
}


bool AmcChunkActor::AddBlock(FBlockDefinition localBlockData, FIntVector BlockTransform, bool bInstantAdd)
{
	int32 tempIndex;

	if (bInstantAdd)
	{
		if (!BlockComponentPlayerData.IsValidIndex(localBlockData.ID))
		{
			BlockComponentPlayerData.Init(FChunkSectionData(), localBlockData.ID + 1);
		}
		tempIndex = BlockComponentPlayerData[localBlockData.ID].BlockTransforms.Find(BlockTransform);
	}
	else
	{
		if (!BlockComponentData.IsValidIndex(localBlockData.ID))
		{
			BlockComponentData.Init(FChunkSectionData(), localBlockData.ID + 1);
			BlockComponentToSpawn.AddUnique(localBlockData.ID);
		}
		tempIndex = BlockComponentData[localBlockData.ID].BlockTransforms.Find(BlockTransform);
	}
	
	if (tempIndex == INDEX_NONE)
	{
		if (!BlockData.Contains(localBlockData.ID))
			BlockData.Add(localBlockData.ID, localBlockData);

		if (bInstantAdd)
		{
			BlockComponentPlayerData[localBlockData.ID].BlockTransforms.Add(BlockTransform);
			GetBlockComponent(localBlockData.ID, true)->AddInstance(FTransform(FVector(BlockTransform)));
			return true;
		}

		if (bFirstLayerGenerated || BlockComponentData[localBlockData.ID].BlockIndexToSpawn.Num() < 256)
			BlockComponentData[localBlockData.ID].BlockIndexToSpawn.Add(BlockComponentData[localBlockData.ID].BlockTransforms.Add(BlockTransform));
		else
			BlockComponentData[localBlockData.ID].DelayedIndexToSpawn.Add(BlockComponentData[localBlockData.ID].BlockTransforms.Add(BlockTransform));

		BlockComponentToSpawn.AddUnique(localBlockData.ID);
	
		return true;
	}
	else if ((bInstantAdd ? BlockComponentPlayerData : BlockComponentData)[localBlockData.ID].DestroyedBlocks.Find(tempIndex) != INDEX_NONE)
	{
		(bInstantAdd ? BlockComponentPlayerData : BlockComponentData)[localBlockData.ID].DestroyedBlocks.RemoveSwap(tempIndex);

		if (bInstantAdd)
		{
			GetBlockComponent(localBlockData.ID, true)->AddInstance(FTransform(FVector((bInstantAdd ? BlockComponentPlayerData : BlockComponentData)[localBlockData.ID].BlockTransforms[tempIndex])));
			return true;
		}
		
		BlockComponentData[localBlockData.ID].BlockIndexToSpawn.Add(tempIndex);
		BlockComponentToSpawn.AddUnique(localBlockData.ID);
		
	}
	return false;
}

bool AmcChunkActor::RemoveBlock(UmcBlockComponent* Block)
{
	//todo
	return false;
}

void AmcChunkActor::TickBlockPool()
{
		if (BlockComponentToSpawn.IsValidIndex(0))
		{
			int32 tempComponentIndex = BlockComponentToSpawn.Last();
			if (BlockComponentData[tempComponentIndex].BlockIndexToSpawn.IsValidIndex(0))
			{
				int32 tempIndex = BlockComponentData[tempComponentIndex].BlockIndexToSpawn.Pop();

				if (!BlockComponentData[tempComponentIndex].DestroyedBlocks.Contains(tempIndex))
				{
					GetBlockComponent(tempComponentIndex)->AddInstance( FTransform( FVector( BlockComponentData[tempComponentIndex].BlockTransforms[tempIndex] ) ) );
				}

				if (tempIndex == 0 && BlockComponentData[tempComponentIndex].DelayedIndexToSpawn.IsValidIndex(0))
				{
					BlockComponentData[tempComponentIndex].BlockIndexToSpawn = BlockComponentData[tempComponentIndex].DelayedIndexToSpawn;
					BlockComponentData[tempComponentIndex].DelayedIndexToSpawn.Empty();

					GetWorld()->GetTimerManager().ClearTimer(BlockSpawnerTimer);
					BlockComponentToSpawn.Push(tempComponentIndex);
				}

				return;
			}
			else
			{
				BlockComponentToSpawn.Pop();
				return;
			}
		}
	
	GetWorld()->GetTimerManager().ClearTimer(BlockSpawnerTimer);
	
	return;
}
bool AmcChunkActor::LoadChunk(bool bTemporary)
{
	FIntVector ChunkLocation = LocationToChunkPos(GetActorLocation());

	//temp save is only available during runtime, when ever new game is started temporary data is cleared
	if (UGameplayStatics::DoesSaveGameExist("Temporary/" + FString::FromInt(ChunkLocation.X) + "." + FString::FromInt(ChunkLocation.Y), 0))
	{
		LoadChunk_Internal(true);
		return true;
	}

	if (!bTemporary)
	{
		if (UGameplayStatics::DoesSaveGameExist("Map1/" + FString::FromInt(ChunkLocation.X) + "." + FString::FromInt(ChunkLocation.Y), 0))
		{
			LoadChunk_Internal(false);
			return true;
		}
	}
	
	return false;
}
void AmcChunkActor::LoadChunk_Internal(bool bTemporary)
{
	FIntVector ChunkLocation = LocationToChunkPos(GetActorLocation());

	UmcChunkSave* SaveGameInstance = Cast<UmcChunkSave>(UGameplayStatics::LoadGameFromSlot((bTemporary ? "Temporary/" : "Map1/") + FString::FromInt(ChunkLocation.X) + "." + FString::FromInt(ChunkLocation.Y), 0));
	BlockComponentData = SaveGameInstance->BlockComponentData;
	BlockComponentPlayerData = SaveGameInstance->BlockComponentPlayerData;
	BlockData = SaveGameInstance->BlockData;

	for (int32 x = 0; x < BlockComponentData.Num(); x++)
	{
		for (int32 y = 0; y < BlockComponentData[x].BlockTransforms.Num(); y++)
			BlockComponentData[x].BlockIndexToSpawn.Add(y);
		
		BlockComponentToSpawn.AddUnique(x);
	}
	for (int32 x = 0; x < BlockComponentPlayerData.Num(); x++)
	{
		for (int32 y = 0; y < BlockComponentPlayerData[x].BlockTransforms.Num(); y++)
			GetBlockComponent(x , true)->AddInstance(FTransform(FVector(BlockComponentPlayerData[x].BlockTransforms[y])));
	}

}

void AmcChunkActor::SaveChunk()
{
	UmcChunkSave* SaveGameInstance = Cast<UmcChunkSave>(UGameplayStatics::CreateSaveGameObject(UmcChunkSave::StaticClass()));
	SaveGameInstance->BlockComponentData = BlockComponentData;
	SaveGameInstance->BlockComponentPlayerData = BlockComponentPlayerData;
	SaveGameInstance->BlockData = BlockData;

	FIntVector ChunkLocation = LocationToChunkPos(GetActorLocation());
		
	//remove any "removed blocks" before saving, we also need to make sure we go backwards so that we remove the blocks in correct order as removal in middle of array would cause reordering
	//so we sort them, and remove from end to ensure that we delete the correct block and keep the order of the array
	for (int32 x = 0; x < SaveGameInstance->BlockComponentData.Num(); x++)
	{
		SaveGameInstance->BlockComponentData[x].DestroyedBlocks.Sort();
		while (SaveGameInstance->BlockComponentData[x].DestroyedBlocks.IsValidIndex(0))
		{
			SaveGameInstance->BlockComponentData[x].BlockTransforms.RemoveAt(SaveGameInstance->BlockComponentData[x].DestroyedBlocks.Pop(), 1, false);
		}
		SaveGameInstance->BlockComponentData[x].BlockTransforms.Shrink();
	}

	for (int32 x = 0; x < SaveGameInstance->BlockComponentPlayerData.Num(); x++)
	{
		int32 tempIndex = 0;
		SaveGameInstance->BlockComponentPlayerData[x].DestroyedBlocks.Sort();
		while (SaveGameInstance->BlockComponentPlayerData[x].DestroyedBlocks.IsValidIndex(0))
		{
			tempIndex = SaveGameInstance->BlockComponentPlayerData[x].DestroyedBlocks.Pop();
			SaveGameInstance->BlockComponentPlayerData[x].BlockTransforms.RemoveAt(tempIndex, 1, false);
		}
		SaveGameInstance->BlockComponentPlayerData[x].BlockTransforms.Shrink();
	}

	FString SavePath = "Temporary/" + FString::FromInt(ChunkLocation.X) + "." + FString::FromInt(ChunkLocation.Y);
		
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SavePath, 0);
}

void AmcChunkActor::GenerateNewChunk()
{
	GetWorld()->GetTimerManager().SetTimer(GenerationTimer, this, &AmcChunkActor::GenerateNewChunk_Internal, 0.3f , false);
}

void AmcChunkActor::GenerateNewChunk_Internal()
{
	for (uint8 x = 0; x < BlockComponentData.Num(); x++)
	{
		FChunkSectionData tempElement = BlockComponentData[x];
		ParallelFor(tempElement.BlockTransforms.Num(), [&](int32 Index)
		{
			FBlockDefinition tempBlockData = BlockData[x];
			FIntVector tempBlockTransform = tempElement.BlockTransforms[Index];

			for (int z = 1; z < 3; z++)
			{
				AddBlock(tempBlockData, tempBlockTransform - FIntVector(0, 0, z*BlockSize));
			}
		}, true);
	}
}

void AmcChunkActor::BeginDestroy()
{
	if(GetWorld())
		GetWorld()->GetTimerManager().ClearTimer(BlockSpawnerTimer);
	Super::BeginDestroy();
}

AmcInteractableBlock* AmcChunkActor::InteractWithBlock(UInstancedStaticMeshComponent* BlockComponent, int32 Instance)
{
	UmcBlockComponent* tempBlockComponent = Cast<UmcBlockComponent>(BlockComponent);

	FBlockDefinition tempData = Cast<UmcSingleton>(GEngine->GameSingleton)->getBlockData(tempBlockComponent->ID);
	FTransform tempTransform = FTransform();
	AmcInteractableBlock* tempIntBlock;

	if(tempBlockComponent->GetInstanceCount() >= Instance)

	tempBlockComponent->GetInstanceTransform(Instance, tempTransform, true);

	if (GetWorld())
	{
		tempIntBlock = GetWorld()->SpawnActor<AmcInteractableBlock>(AmcInteractableBlock::StaticClass(), tempTransform, FActorSpawnParameters());
		if (tempIntBlock)
		{
			tempIntBlock->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			tempIntBlock->InitBlock(tempData, BlockPlayerComponents.FindKey(tempBlockComponent)!=nullptr);
			tempIntBlock->onInteractionFinished.AddDynamic(this, &AmcChunkActor::onInteractionFinished);

			tempBlockComponent->RemoveInstance(Instance);

			return tempIntBlock;
		}
	}
	return NULL;
}

void AmcChunkActor::onInteractionFinished(FVector Location, uint8 ChunkBlockID, bool bIsDestroyed)
{
	bool bPlayerModification = true;

	if (!BlockComponentPlayerData.IsValidIndex(ChunkBlockID))
	{
		BlockComponentPlayerData.Init(FChunkSectionData(), ChunkBlockID + 1);
	}
	int32 tempIndex = BlockComponentPlayerData[ChunkBlockID].BlockTransforms.Find(FIntVector(Location - GetActorLocation()));
	
	if (tempIndex == INDEX_NONE)
	{
		tempIndex = BlockComponentData[ChunkBlockID].BlockTransforms.Find(FIntVector(Location - GetActorLocation()));
		bPlayerModification = false;
	}

	if (bIsDestroyed)
	{
		(bPlayerModification ? BlockComponentPlayerData : BlockComponentData)[ChunkBlockID].DestroyedBlocks.Add(tempIndex);
	}
	else
	{
		GetBlockComponent(ChunkBlockID, bPlayerModification)->AddInstance(FTransform(FVector((bPlayerModification ? BlockComponentPlayerData : BlockComponentData)[ChunkBlockID].BlockTransforms[tempIndex])));
	}
}

UmcBlockComponent* AmcChunkActor::GetBlockComponent(int32 ComponentIndex, bool bPlayerModification)
{
	if (bPlayerModification)
	{
		if (BlockPlayerComponents.Contains(ComponentIndex)) //check if we have a component for this "block id"
		{
			return BlockPlayerComponents[ComponentIndex];
		}
	}
	else
	{
		if (BlockComponents.Contains(ComponentIndex)) //check if we have a component for this "block id"
		{
			return BlockComponents[ComponentIndex];
		}
	}

	UmcBlockComponent* tempBlockComp = NewObject<UmcBlockComponent>(this);

	tempBlockComp->RegisterComponent();
	tempBlockComp->AttachTo(RootComponent);
	tempBlockComp->InitBlock(BlockData[ComponentIndex], bPlayerModification);

	(bPlayerModification ? BlockPlayerComponents : BlockComponents).Add(ComponentIndex, tempBlockComp);

	return tempBlockComp;
}