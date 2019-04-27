// Fill out your copyright notice in the Description page of Project Settings.


#include "mcSingleton.h"

UmcSingleton::UmcSingleton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FBlockDefinition UmcSingleton::getBlockData_Implementation(uint8 ID)
{
	return FBlockDefinition();
}

UMaterialInterface* UmcSingleton::getDefaultMaterial_Implementation()
{
	return NULL;
}
UStaticMesh* UmcSingleton::getDefaultMesh_Implementation()
{
	return NULL;
}