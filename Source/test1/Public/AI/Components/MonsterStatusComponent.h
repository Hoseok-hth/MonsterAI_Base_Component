#pragma once

#include "CoreMinimal.h"
#include "AI/Type/EMonsterState.h"
#include "AI/Navigation/MonsterPatrolPoint.h"
#include "Components/ActorComponent.h"
#include "MonsterStatusComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEST1_API UMonsterStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UMonsterStatusComponent();
	
	//monster idle type & patrol location
	//몬스터가 idle일때 행동 분류 & patrol Actor array 반환함수
	EIdleBehavior GetIdleBehavior() const { return IdleBehavior; }
	const TArray<AMonsterPatrolPoint*>& GetPatrolTargets() const { return PatrolPoints; }
	
	//도착 판정 범위 반환
	float GetArrivalRadius() const { return ArrivalRadius; }
	
	// monster speed 반환
	float GetCurrentSpeed() const { return BaseSpeed * SpeedMultiplier; }
	void SetSpeedMultiplier(float NewMultiplier) { SpeedMultiplier = NewMultiplier; }
    float GetBaseSpeed() const { return BaseSpeed; }
	float GetChaseSpeed() const {return ChaseSpeed; }
	// monster sensing
	float GetBaseDetectionRange() const { return BaseDetectionRange; }
	float GetViewAngle() const { return ViewAngle; }
	float GetBaseHearingRange() const { return BaseHearingRange; }
	
	float GetMenaceDuration() const { return MenaceDuration; }
	
	//monster type
	EMonsterType GetMonsterType() const { return MonsterType; }

	//monster attack
	float GetAttackRange() const { return AttackRange; }
	
	//monster chase
	float GetChaseRange() const { return ChaseRange; }
private:
	//default = visual sense
	//기본값은 시야기반 감지
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	EMonsterType MonsterType = EMonsterType::Visual;
	
	//default = static(don't patrol)
	//idle 기본 행동은 patrol없이 가만 멈춰있음
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	EIdleBehavior IdleBehavior =EIdleBehavior::Static;
	
	//patrol location points(actor)
	UPROPERTY(EditAnywhere, Category = "Monster Status | Patrol")
	TArray<AMonsterPatrolPoint*> PatrolPoints;
	
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	float ArrivalRadius = 100.f;
	
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	float BaseSpeed = 450.f;
	
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	float ChaseSpeed = 700.f;

	UPROPERTY(VisibleAnywhere, Category = "Monster Status")
	float SpeedMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Monster Status")
	float BaseDetectionRange = 1200.f;
	
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	float ViewAngle = 90.f;
	
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	float BaseHearingRange = 1500.f;
	
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	float AttackRange = 150.f;
	
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	float ChaseRange = 2000.f;
	
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	float MenaceDuration = 3.0f;
};