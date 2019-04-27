// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "mcData.h"
#include "Components/ActorComponent.h"
#include "mcChunkActorComp.generated.h"

//forward declare class (preventing possible cyclic dependancy issues)
//required class header is then included in the cpp
class UmcBlockComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINECRAFT_CLONE_API UmcChunkActorComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UmcChunkActorComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	TArray<UmcBlockComponent*> Blocks;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool addBlock(FBlockDefinition BlockData, FByteTransform BlockTransform);
	bool removeBlock(UmcBlockComponent* BlockReference);
			
};
