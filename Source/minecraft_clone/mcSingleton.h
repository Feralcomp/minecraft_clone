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

	//implemented in bp to retrive data from datatable
	UFUNCTION(BlueprintNativeEvent, Category = "Game Data")
	FBlockDefinition getBlockData(uint8 ID);

	//implemented in bp to return basic cube material
	UFUNCTION(BlueprintNativeEvent, Category = "Game Data")
	UMaterialInterface* getDefaultMaterial(bool PlayerPlaced = false);

	//implemented in bp to return basic mesh (incase one isnt specified within block definiton)
	UFUNCTION(BlueprintNativeEvent, Category = "Game Data")
		UStaticMesh* getDefaultMesh();
	
};
