#pragma once

#include "CoreMinimal.h"
#include "EMonsterState.generated.h"

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
	None		UMETA(DisplayName = "None"),
	Idle        UMETA(DisplayName = "Idle"),
	Chase       UMETA(DisplayName = "Chase"),
	Attack      UMETA(DisplayName = "Attack"),
	Menace     UMETA(DisplayName = "Menace"),
	Special     UMETA(DisplayName = "Special"),
	Stunned     UMETA(DisplayName = "Stunned"),
	Dead        UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EIdleBehavior : uint8
{
	Static      UMETA(DisplayName = "Static"),
	Patrol      UMETA(DisplayName = "Patrol")
};

UENUM(BlueprintType) //이거는 몬스터가 시야기반 탐지인지 소리기반인지 둘다인지 혹은 둘다 아니고 특정 트리거 기반인지 나눈 enum
enum class EMonsterType : uint8
{
	Visual      UMETA(DisplayName = "have Visual sense"),   
	Acoustic    UMETA(DisplayName = "have Acoustic sense"), 
	Hybrid      UMETA(DisplayName = "have Visual & Acoustic sense"),        
	Trigger     UMETA(DisplayName = "Only Chase by trigger")   
};