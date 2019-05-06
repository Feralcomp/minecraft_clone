// Fill out your copyright notice in the Description page of Project Settings.


#include "mcCharacter.h"

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
}

// Called every frame
void AmcCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentIntBlock)
	{
		FHitResult OutHit;
		OutHit = TraceForBlock();

		if (OutHit.Actor != CurrentIntBlock)
		{
			CurrentIntBlock->Interaction_Stop();
			CurrentIntBlock = NULL;
		}
	}

}

// Called to bind functionality to input
void AmcCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
FHitResult AmcCharacter::TraceForBlock()
{
	FHitResult OutHit;
	FVector StartVector;
	FRotator StartRotation;

	GetActorEyesViewPoint(StartVector, StartRotation);

	// alternatively you can get the camera location
	// FVector Start = FirstPersonCameraComponent->GetComponentLocation();

	FVector End = ((StartRotation.Vector() * 500) + StartVector);
	FCollisionQueryParams CollisionParams;

	GetWorld()->LineTraceSingleByChannel(OutHit, StartVector, End, ECC_Visibility, CollisionParams);

	return OutHit;
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
}

void AmcCharacter::TryPlaceBlock()
{

}
