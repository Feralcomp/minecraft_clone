// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "mcData.h"
#include "UObject/NoExportTypes.h"
#include "mcSingleton.generated.h"

/**
 * this class is extended in blueprints to allow for easy asset referencing
 * as a singleton this class is accessible almost anywhere.
 */
UCLASS(Blueprintable)
class MINECRAFT_CLONE_API UmcSingleton : public UObject
{
		GENERATED_UCLASS_BODY()

	public:

	UFUNCTION(BlueprintNativeEvent, Category = "Game Data")
	FBlockDefinition getBlockData(uint8 ID);

	UFUNCTION(BlueprintNativeEvent, Category = "Game Data")
	UMaterialInterface* getDefaultMaterial();

	UFUNCTION(BlueprintNativeEvent, Category = "Game Data")
	UStaticMesh* getDefaultMesh();
	
};
