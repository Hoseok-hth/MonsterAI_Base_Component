// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AI/Type/EMonsterState.h"
#include "MonsterAnimInstance.generated.h"


/**
 * 
 */
UCLASS()
class TEST1_API UMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category = "Monster Info")
	float CurrentSpeed; 

	UPROPERTY(BlueprintReadOnly, Category = "Monster Info")
	bool bIsStunned; 

	UPROPERTY(BlueprintReadOnly, Category = "Monster Info")
	EMonsterState State; 
};
