// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "mcData.h"
#include "mcSingleton.h"
#include "Components/StaticMeshComponent.h"
#include "mcInteractableBlock.generated.h"

class AmcChunkActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FBlockInteraction, FVector, Location, uint8, ID, bool, bDestroyed);

UCLASS()
class MINECRAFT_CLONE_API AmcInteractableBlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AmcInteractableBlock();

	UStaticMeshComponent* BlockMesh;

	uint8 Hardness = 0;

	uint8 BlockID;

	FBlockInteraction onInteractionFinished;

	FTimerHandle BlockInteractionTimer = FTimerHandle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitBlock(FBlockDefinition BlockData);

	void ConsumeBlock();
	void Interaction_Start();
	void Interaction_Stop(bool bIsDestroyed=false);

};
