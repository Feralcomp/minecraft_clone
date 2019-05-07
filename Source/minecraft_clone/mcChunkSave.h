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
	TArray<FChunkSectionData> BlockComponentData;
	TArray<FChunkSectionData> BlockComponentPlayerData;

	TMap<uint8, FBlockDefinition> BlockData;
	
};
