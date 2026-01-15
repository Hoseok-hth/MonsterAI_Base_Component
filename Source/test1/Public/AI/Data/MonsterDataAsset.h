// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Type/EMonsterState.h"
#include "Engine/DataAsset.h"
#include "MonsterDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TEST1_API UMonsterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	//default = visual sense
	//기본값은 시야기반 감지
	UPROPERTY(EditAnywhere, Category = "Sense Type")
	EMonsterType MonsterType = EMonsterType::Visual;
	
	UPROPERTY(EditAnywhere, Category = "Chase")
	float ArrivalRadius = 100.f;
	
	UPROPERTY(EditAnywhere, Category = "Speed")
	float BaseSpeed = 450.f;
	
	UPROPERTY(EditAnywhere, Category = "Speed")
	float ChaseSpeed = 700.f;

	UPROPERTY(EditAnywhere, Category = "Range")
	float BaseDetectionRange = 1200.f;
	
	UPROPERTY(EditAnywhere, Category = "Angle")
	float ViewAngle = 90.f;
	
	UPROPERTY(EditAnywhere, Category = "Range")
	float BaseHearingRange = 1500.f;
	
	UPROPERTY(EditAnywhere, Category = "Range")
	float AttackRange = 150.f;
	
	UPROPERTY(EditAnywhere, Category = "Range")
	float ChaseRange = 2000.f;
	
	UPROPERTY(EditAnywhere, Category = "Duration")
	float MenaceDuration = 3.0f;
	
	UPROPERTY(EditAnywhere, Category = "Spec")
	float EyeHeight = 70.f;
	
	UPROPERTY(EditAnywhere, Category = "Duration")
	float MaxLostTargetTime = 5.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* ExecutionMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* StunnedMontage;
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* WalkSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* RunSound;
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* IdleSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* ChaseSound;
	
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* HowlingSound;
	
	UPROPERTY(EditAnywhere, Category = "Volume")
	float IdleSoundVolume = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Volume")
	float ChaseSoundVolume = 1.0f;
};
