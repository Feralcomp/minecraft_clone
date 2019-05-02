// Fill out your copyright notice in the Description page of Project Settings.


#include "mcChunkActor.h"

// Sets default values for this component's properties
AmcChunkActor::AmcChunkActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bRunOnAnyThread = true;
	PrimaryActorTick.TickInterval = 0.005;

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
			GetWorld()->GetTimerManager().SetTimer(BlockSpawnerTimer, this, &AmcChunkActor::TickBlockPool, float(1.0/(float(ChunkSize.X*ChunkSize.Y) * (bFirstLayerGenerated ? 0.5 : 2.0 ) )), true,FMath::FRand());
	}
}


bool AmcChunkActor::AddBlock(FBlockDefinition localBlockData, FIntVector BlockTransform)
{
	if (BlockTransforms.Find(BlockTransform) == INDEX_NONE)
	{
		if(!BlockData.Contains(localBlockData.ID))
			BlockData.Add(localBlockData.ID, localBlockData);

		BlockIDs.Add(localBlockData.ID);

		if(bFirstLayerGenerated || BlockIndexToSpawn.Num() < 512)
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
		uint8 tempBlockID = BlockIDs[tempIndex];
		if(BlockComponents.Contains(tempBlockID)) //check if we have a component for this "block id"
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
	for (int x = 0; x < ChunkSize.X*ChunkSize.Y; x++)
	{
		FBlockDefinition tempBlockData = BlockData[BlockIDs[x]];
		FIntVector tempBlockTransform = BlockTransforms[x];

		for (int z = 1; z < 3; z++)
		{
			AddBlock(tempBlockData, tempBlockTransform - FIntVector(0, 0, z*BlockSize));
		}
	}
}

void AmcChunkActor::BeginDestroy()
{
	if(GetWorld())
		GetWorld()->GetTimerManager().ClearTimer(BlockSpawnerTimer);
	Super::BeginDestroy();
}

