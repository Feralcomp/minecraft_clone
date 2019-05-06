// Fill out your copyright notice in the Description page of Project Settings.


#include "mcChunkActor.h"

// Sets default values for this component's properties
AmcChunkActor::AmcChunkActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1;

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

	if (!GetWorld()->GetTimerManager().IsTimerActive(BlockSpawnerTimer))
	{
		if(BlockIndexToSpawn.IsValidIndex(0))
			GetWorld()->GetTimerManager().SetTimer(BlockSpawnerTimer, this, &AmcChunkActor::TickBlockPool, float(1.0/(bFirstLayerGenerated ? 128 : 256)), true,FMath::FRand()*.2);
	}
}


bool AmcChunkActor::AddBlock(FBlockDefinition localBlockData, FIntVector BlockTransform)
{
	if (BlockTransforms.Find(BlockTransform) == INDEX_NONE)
	{
		if(!BlockData.Contains(localBlockData.ID))
			BlockData.Add(localBlockData.ID, localBlockData);

		BlockIDs.Add(localBlockData.ID);

		if(bFirstLayerGenerated || BlockIndexToSpawn.Num() < 1024)
			BlockIndexToSpawn.Add(BlockTransforms.Add(BlockTransform));
		else
			DelayedIndexToSpawn.Add(BlockTransforms.Add(BlockTransform));

		return true;
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
	if (BlockIndexToSpawn.IsValidIndex(0))
	{
		int32 tempIndex = BlockIndexToSpawn.Pop();

		if (!DestroyedBlocks.Contains(tempIndex))
		{
			uint8 tempBlockID = BlockIDs[tempIndex];
			if (BlockComponents.Contains(tempBlockID)) //check if we have a component for this "block id"
			{
				BlockComponents[BlockIDs[tempIndex]]->AddInstance(FTransform(FVector(BlockTransforms[tempIndex])));
			}
			else
			{
				UmcBlockComponent* tempBlockComp = NewObject<UmcBlockComponent>(this);

				tempBlockComp->RegisterComponent();
				tempBlockComp->AttachTo(RootComponent);
				tempBlockComp->InitBlock(BlockData[tempBlockID]);
				tempBlockComp->AddInstance(FTransform(FVector(BlockTransforms[tempIndex])));

				BlockComponents.Add(tempBlockID, tempBlockComp);
			}
		}

		if (!bFirstLayerGenerated && tempIndex == 0)
		{
			bFirstLayerGenerated = true;
			BlockIndexToSpawn = DelayedIndexToSpawn;
			DelayedIndexToSpawn.Empty();
			GetWorld()->GetTimerManager().ClearTimer(BlockSpawnerTimer);
		}

		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(BlockSpawnerTimer);
	return;
}
bool AmcChunkActor::LoadChunk()
{
	//todo try loading
	//if not loaded generate
	return false;
}
void AmcChunkActor::LoadChunk_Internal()
{}

void AmcChunkActor::GenerateNewChunk()
{
	GetWorld()->GetTimerManager().SetTimer(GenerationTimer, this, &AmcChunkActor::GenerateNewChunk_Internal, 0.1f , false);
}

void AmcChunkActor::GenerateNewChunk_Internal()
{
	ParallelFor(ChunkSize.X*ChunkSize.Y, [&](int32 Index)
	{
		FBlockDefinition tempBlockData = BlockData[BlockIDs[Index]];
		FIntVector tempBlockTransform = BlockTransforms[Index];

		for (int z = 1; z < 3; z++)
		{
			AddBlock(tempBlockData, tempBlockTransform - FIntVector(0, 0, z*BlockSize));
		}
	}, true);
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


	tempBlockComponent->GetInstanceTransform(Instance, tempTransform, true);

	if (GetWorld())
	{
		tempIntBlock = GetWorld()->SpawnActor<AmcInteractableBlock>(AmcInteractableBlock::StaticClass(), tempTransform, FActorSpawnParameters());
		if (tempIntBlock)
		{
			tempIntBlock->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			tempIntBlock->InitBlock(tempData);
			tempIntBlock->onInteractionFinished.AddDynamic(this, &AmcChunkActor::onInteractionFinished);
			return tempIntBlock;
		}
	}
	return NULL;
}

void AmcChunkActor::onInteractionFinished(FVector Location, uint8 ChunkBlockID, bool bIsDestroyed)
{
	int32 tempID = BlockTransforms.Find(FIntVector(Location - GetActorLocation()));

	if (bIsDestroyed)
	{
		DestroyedBlocks.Add(tempID);
	}
	else
		BlockIndexToSpawn.Add(tempID);
}