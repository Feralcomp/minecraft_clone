// Fill out your copyright notice in the Description page of Project Settings.


#include "mcBlockComponent.h"

// Sets default values for this component's properties
UmcBlockComponent::UmcBlockComponent()
{
}



void UmcBlockComponent::InitBlock(FBlockDefinition BlockDefinition)
{
	SetStaticMesh(BlockDefinition.BlockMesh);
	UMaterialInstanceDynamic* tempMaterial = UMaterialInstanceDynamic::Create(Cast<UmcSingleton>(GEngine->GameSingleton)->getDefaultMaterial(), this);
	tempMaterial->SetTextureParameterValue(FName("Diffuse"), BlockDefinition.BlockTexture);
	SetMaterial(0, tempMaterial);
}
