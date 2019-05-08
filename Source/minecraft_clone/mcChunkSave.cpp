// Fill out your copyright notice in the Description page of Project Settings.


#include "mcChunkSave.h"

UmcChunkSave::UmcChunkSave()
{
	//for weird reason empty arrays refuse to save, save will become corrupted and unloadable
	//apparently reserving a space for it and making it a UPROPERTY ensures correct saving
	BlockComponentData.Reserve(1);
	BlockComponentPlayerData.Reserve(1);

	BlockData.Reserve(4);

	bool bInitiallyLoaded = false;
}
