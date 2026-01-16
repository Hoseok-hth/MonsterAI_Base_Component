// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "BackRoomAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class TEST1_API UBackRoomAssetManager : public UAssetManager
{
	GENERATED_BODY()
	public:
	virtual void StartInitialLoading() override;
};
