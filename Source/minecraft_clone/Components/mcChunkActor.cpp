// Fill out your copyright notice in the Description page of Project Settings.


#include "mcChunkActor.h"

// Sets default values for this component's properties
AmcChunkActor::AmcChunkActor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;

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

	// ...
}


bool AmcChunkActor::AddBlock(FBlockDefinition BlockData, FIntVector BlockTransform)
{
	UmcBlockComponent* tempBlockComp = CreateDefaultSubobject<UmcBlockComponent>(TEXT("Block"));
	
	tempBlockComp->SetRelativeLocation(FVector(BlockTransform.X, BlockTransform.Y, BlockTransform.Z));
	tempBlockComp->InitBlock(BlockData);

	Blocks.AddUnique(tempBlockComp);


	return IsValid(tempBlockComp);
}

bool AmcChunkActor::RemoveBlock(UmcBlockComponent* Block)
{
	return Blocks.Remove(Block);
}
