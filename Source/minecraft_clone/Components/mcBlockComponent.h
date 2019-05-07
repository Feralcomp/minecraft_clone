// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "minecraft_clone.h"
#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "mcData.h"

#include "mcBlockComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINECRAFT_CLONE_API UmcBlockComponent : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UmcBlockComponent();

	uint8 ID;
	uint8 Hardness;

	UFUNCTION(BlueprintCallable)
	void InitBlock(FBlockDefinition BlockDefinition, bool bPlayerModification = false);

	
};
