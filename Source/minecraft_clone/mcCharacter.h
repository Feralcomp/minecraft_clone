// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "mcSingleton.h"
#include "mcInteractableBlock.h"
#include "Components/mcChunkActor.h"
#include "mcCharacter.generated.h"

UCLASS()
class MINECRAFT_CLONE_API AmcCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AmcCharacter();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Interaction")
	bool bPlacementMode = false;

	bool bShouldInteract = false;

	APlayerController* CastedPlayerController;

	uint8 CurrentPlacementBlockID = -1;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Interaction")
	AmcInteractableBlock* CurrentIntBlock;

	UmcSingleton* CastedSingleton;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void Turn(float Rate);

	void LookUp(float Rate);


	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

	void StartInteraction();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void StopInteraction();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryPlaceBlock();

	FHitResult TraceForBlock();

	void CancelPlacement();

	void ShowBlockOutline();

	void HandleInteraction();

	//////////////////////
	void SelectBlockType(uint8 BlockID);

	APlayerController* GetPlayerControllerRef();

	void SaveGame();
	
	template<uint8 Index>
	void SelectBlockType()
	{
		SelectBlockType(Index);
	}
	/////////////////////

	
	void ExitGame();
	

};
