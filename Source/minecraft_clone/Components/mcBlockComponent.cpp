// Fill out your copyright notice in the Description page of Project Settings.


#include "mcBlockComponent.h"

// Sets default values for this component's properties
UmcBlockComponent::UmcBlockComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


//initiate the block, set the mesh and material and hardness
void UmcBlockComponent::InitBlock(FBlockDefinition BlockDefinition)
{
	//local temp var for easier access
	FBlockTexture tempTexture = BlockDefinition.BlockTexture;

	//set block mesh
	SetStaticMesh(BlockDefinition.BlockMesh);

	//create dynamic material instance and set texture parameters
	UMaterialInstanceDynamic* tempMaterial = UMaterialInstanceDynamic::Create(Cast<UmcSingleton>(GEngine->GameSingleton)->getDefaultMaterial(), this);
	
	//set side texture
	tempMaterial->SetTextureParameterValue(FName("Side"), tempTexture.Side);

	//set top texture (if available) otherwise use side texture
	tempMaterial->SetTextureParameterValue(FName("Top"), tempTexture.Top ? tempTexture.Top : tempTexture.Side);
	
	//set bottom texture (if available) otherwise use side texture
	tempMaterial->SetTextureParameterValue(FName("Bottom"), tempTexture.Bottom ? tempTexture.Bottom : tempTexture.Side);
	
	//apply desired material to mesh
	SetMaterial(0, tempMaterial);
}
