// MonsterPatrolPoint.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterPatrolPoint.generated.h"

UCLASS()
class TEST1_API AMonsterPatrolPoint : public AActor
{
	GENERATED_BODY()

public:
	AMonsterPatrolPoint();

	// 이 지점에서 대기할 시간 (0이면 바로 통과)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	float WaitTime = 0.0f;

	
	UPROPERTY(EditAnywhere, Category = "Patrol")
	class UAnimMontage* WaitAnimation;
};