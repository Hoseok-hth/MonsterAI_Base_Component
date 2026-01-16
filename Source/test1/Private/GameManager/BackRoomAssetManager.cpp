// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager/BackRoomAssetManager.h"
#include "Global/BackRoomTags.h"


void UBackRoomAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	Super::StartInitialLoading();

	
	FBackRoomTags::InitializeNativeTags();
}
