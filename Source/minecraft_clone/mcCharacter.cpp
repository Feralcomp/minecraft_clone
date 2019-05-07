// Fill out your copyright notice in the Description page of Project Settings.


#include "mcCharacter.h"
#include "mcWorldManager.h"

// Sets default values
AmcCharacter::AmcCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AmcCharacter::BeginPlay()
{
	Super::BeginPlay();

	CastedSingleton = Cast<UmcSingleton>(GEngine->GameSingleton);
	if (IsValid(GetWorld()) && GetWorld() != NULL)
	{
		UmcGameInstance * tempGameInstance = Cast<UmcGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (IsValid(tempGameInstance) && tempGameInstance->bShouldLoadSave)
		{
			UmcSaveGame* LoadGameInstance = Cast<UmcSaveGame>(UGameplayStatics::CreateSaveGameObject(UmcSaveGame::StaticClass()));
			LoadGameInstance = Cast<UmcSaveGame>(UGameplayStatics::LoadGameFromSlot("map1", 0));
			SetActorLocation(LoadGameInstance->PlayerLocation);
		}
	}
}

// Called every frame
void AmcCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentIntBlock)
	{
		FHitResult OutHit = TraceForBlock();

		if (OutHit.Actor != CurrentIntBlock)
		{
			CurrentIntBlock->Interaction_Stop();
			CurrentIntBlock = NULL;
		}
	}
	else
	{
		if (bShouldInteract)
		{
			TryInteract();
		}
	}
	ShowBlockOutline();

}

void AmcCharacter::ShowBlockOutline()
{
	FHitResult OutHit = TraceForBlock();
	if (OutHit.bBlockingHit)
	{
		FVector tempLocation = OutHit.Location + ((OutHit.ImpactNormal*(BlockSize / 2)*(bPlacementMode ? 1 : -1)));
		DrawDebugBox(GetWorld(), SnapVectorToGrid(tempLocation), FVector(BlockSize/2), FColor(255, 0, 0), false, GetWorld()->GetDeltaSeconds()+0.01, uint8('\000'), (bPlacementMode ? 5.0 : 1.0));
	}
}

// Called to bind functionality to input
void AmcCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction("Cube1", IE_Pressed, this, &AmcCharacter::SelectBlockType<0>);
	PlayerInputComponent->BindAction("Cube2", IE_Pressed, this, &AmcCharacter::SelectBlockType<1>);
	PlayerInputComponent->BindAction("Cube3", IE_Pressed, this, &AmcCharacter::SelectBlockType<2>);
	PlayerInputComponent->BindAction("Cube4", IE_Pressed, this, &AmcCharacter::SelectBlockType<3>);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AmcCharacter::HandleInteraction);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AmcCharacter::StopInteraction);

	PlayerInputComponent->BindAction("Place", IE_Pressed, this, &AmcCharacter::TryPlaceBlock);

	PlayerInputComponent->BindAction("Cancel", IE_Pressed, this, &AmcCharacter::CancelPlacement);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AmcCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AmcCharacter::StopJumping);

	PlayerInputComponent->BindAxis("LookUp", this, &AmcCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &AmcCharacter::Turn);

	PlayerInputComponent->BindAxis("MoveForward", this, &AmcCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AmcCharacter::MoveRight);

	PlayerInputComponent->BindAction("SaveGame", IE_Pressed, this, &AmcCharacter::SaveGame);
	PlayerInputComponent->BindAction("Exit", IE_Pressed, this, &AmcCharacter::ExitGame);
}
void AmcCharacter::Turn(float Rate)
{
	
	AddControllerYawInput(Rate * GetPlayerControllerRef()->InputYawScale * GetWorld()->GetDeltaSeconds() * 10);
}

void AmcCharacter::LookUp(float Rate)
{
	AddControllerPitchInput(Rate * GetPlayerControllerRef()->InputPitchScale * GetWorld()->GetDeltaSeconds() * 10);
}
void AmcCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AmcCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

FHitResult AmcCharacter::TraceForBlock()
{
	FHitResult OutHit;
	FVector StartVector = GetActorLocation();
	FRotator StartRotation = GetControlRotation();

	FVector End = ((StartRotation.Vector() * 350) + StartVector);
	FCollisionQueryParams CollisionParams;

	GetWorld()->LineTraceSingleByChannel(OutHit, StartVector, End, ECC_Visibility, CollisionParams);

	return OutHit;
}

void AmcCharacter::HandleInteraction()
{
	if (!bPlacementMode)
	{
		bShouldInteract = true;
		TryInteract();
	}
}

void AmcCharacter::TryInteract()
{
	FHitResult OutHit;
	OutHit = TraceForBlock();
	
	if (OutHit.bBlockingHit)
	{
		if (OutHit.Actor->GetClass()->IsChildOf(AmcInteractableBlock::StaticClass()))
		{
			CurrentIntBlock = Cast<AmcInteractableBlock>(OutHit.Actor);
			StartInteraction();
		}
		else if (OutHit.Actor->GetClass()->IsChildOf(AmcChunkActor::StaticClass()))
		{
			if (OutHit.Component->GetClass()->IsChildOf(UmcBlockComponent::StaticClass()))
			{
				CurrentIntBlock = Cast<AmcChunkActor>(OutHit.Actor)->InteractWithBlock(Cast<UInstancedStaticMeshComponent>(OutHit.Component), OutHit.Item);
				StartInteraction();
			}
		}
	}
}

void AmcCharacter::StartInteraction()
{
	if (CurrentIntBlock)
	{
		CurrentIntBlock->Interaction_Start();
	}
}

void AmcCharacter::StopInteraction()
{
	if (CurrentIntBlock)
	{
		CurrentIntBlock->Interaction_Stop();
	}
	bShouldInteract = false;
}

void AmcCharacter::TryPlaceBlock()
{
	if (!bPlacementMode)
		return;

	FHitResult OutHit;
	OutHit = TraceForBlock();

	if (OutHit.bBlockingHit)
	{
		if (OutHit.Actor->GetClass()->IsChildOf(AmcChunkActor::StaticClass()))
		{
			if (OutHit.Component->GetClass()->IsChildOf(UmcBlockComponent::StaticClass()))
			{
				FIntVector tempLocation = FIntVector(SnapVectorToGrid(OutHit.Location - OutHit.Actor->GetActorLocation() + (OutHit.ImpactNormal*(BlockSize / 2))));

				Cast<AmcChunkActor>(OutHit.Actor)->AddBlock(CastedSingleton->getBlockData(CurrentPlacementBlockID), tempLocation, true);
			}
		}
	}
}

void AmcCharacter::CancelPlacement()
{
	bPlacementMode = false;
}

void AmcCharacter::SelectBlockType(uint8 BlockID)
{
	CurrentPlacementBlockID = BlockID;
	bPlacementMode = true;
}

APlayerController* AmcCharacter::GetPlayerControllerRef()
{
	if (CastedPlayerController != GetController())
		CastedPlayerController = Cast<APlayerController>(GetController());

	return CastedPlayerController;
}

void AmcCharacter::SaveGame()
{
	//find world manager
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AmcWorldManager::StaticClass(), FoundActors);

	if (FoundActors.IsValidIndex(0))
	{
		UmcSaveGame* SaveGameInstance = Cast<UmcSaveGame>(UGameplayStatics::CreateSaveGameObject(UmcSaveGame::StaticClass()));
		SaveGameInstance->WorldSeed = Cast<AmcWorldManager>(FoundActors[0])->WorldSeed;
		SaveGameInstance->PlayerLocation = GetActorLocation();
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, "Map1", 0);
	}
}

void AmcCharacter::ExitGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenuLevel");
}