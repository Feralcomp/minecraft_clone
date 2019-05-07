// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "mcGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MINECRAFT_CLONE_API UmcGameInstance : public UGameInstance
{
	GENERATED_BODY()

	public:
		UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "GameSave")
		bool bShouldLoadSave = false;
	
};
