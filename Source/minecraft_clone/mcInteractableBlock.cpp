// Fill out your copyright notice in the Description page of Project Settings.


#include "mcInteractableBlock.h"

// Sets default values
AmcInteractableBlock::AmcInteractableBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMeshComponent"));
	RootComponent = BlockMesh;
}

// Called when the game starts or when spawned
void AmcInteractableBlock::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AmcInteractableBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AmcInteractableBlock::InitBlock(FBlockDefinition BlockData, bool bPlayerPlaced)
{
	//set block mesh
	BlockMesh->SetStaticMesh(BlockData.BlockMesh);

	FBlockTexture tempTexture = BlockData.BlockTexture;

	//create dynamic material instance and set texture parameters
	UMaterialInstanceDynamic* tempMaterial = UMaterialInstanceDynamic::Create(Cast<UmcSingleton>(GEngine->GameSingleton)->getDefaultMaterial(bPlayerPlaced), this);

	BlockID = BlockData.ID;
	Hardness = BlockData.Hardness;

	//set side texture
	tempMaterial->SetTextureParameterValue(FName("Side"), tempTexture.Side);

	//set top texture (if available) otherwise use side texture
	tempMaterial->SetTextureParameterValue(FName("Top"), tempTexture.Top ? tempTexture.Top : tempTexture.Side);

	//set bottom texture (if available) otherwise use side texture
	tempMaterial->SetTextureParameterValue(FName("Bottom"), tempTexture.Bottom ? tempTexture.Bottom : tempTexture.Side);

	//apply desired material to mesh
	BlockMesh->SetMaterial(0, tempMaterial);
}

void AmcInteractableBlock::ConsumeBlock()
{
	Interaction_Stop(true);
}

void AmcInteractableBlock::Interaction_Start()
{
	if (Hardness == 0)
	{
		ConsumeBlock();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(BlockInteractionTimer, this, &AmcInteractableBlock::ConsumeBlock, float(Hardness));
	}
}

void AmcInteractableBlock::Interaction_Stop(bool bIsDestroyed)
{
	if(IsValid(GetWorld()))
		GetWorld()->GetTimerManager().ClearTimer(BlockInteractionTimer);

	onInteractionFinished.Broadcast(GetActorLocation(),BlockID, bIsDestroyed);
	onInteractionFinished.Clear();
	Destroy();
}

