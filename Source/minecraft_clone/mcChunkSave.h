// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "mcData.h"
#include "mcChunkSave.generated.h"

/**
 * 
 */
UCLASS()
class MINECRAFT_CLONE_API UmcChunkSave : public USaveGame
{
	GENERATED_BODY()

public:
	UmcChunkSave();

	UPROPERTY()
	TArray<FChunkSectionData> BlockComponentData;
	UPROPERTY()
	TArray<FChunkSectionData> BlockComponentPlayerData;
	UPROPERTY()
	TMap<uint8, FBlockDefinition> BlockData;
	
};
