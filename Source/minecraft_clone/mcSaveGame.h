// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "mcSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class MINECRAFT_CLONE_API UmcSaveGame : public USaveGame
{
	GENERATED_BODY()

	public:

	//data that we want to save
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Save Data")
	int32 WorldSeed; //world is generated from noise using seed, reusing seed value lets us generate same world each time.

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Save Data")
	FVector PlayerLocation; //stores players location in the world

	//unfinished, no player inventory was implemented, player has unlimited type of each of the 4 blocks specified in task requirement

	//TArray<uint8> PlayerCubes; //indexes for these two arrays match, this one represents the ID specifiers of available cubes to the player
	//TArray<int32> PlayerCubes_Amount; //this one represents the number of cubes available to player, corresponds to index in PlayerCubes array
};
