#pragma once

#include "CoreMinimal.h"
#include "AI/Type/EMonsterState.h"
#include "Components/ActorComponent.h"
#include "MonsterStatusComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEST1_API UMonsterStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	//status에서 몬스터의 속도, 시야 종류, 시야 범위, 청각 범위, 를 저장하고 다른 컴포넌트 or 몬스터에서 사용
	UMonsterStatusComponent();
	
	//monster idle type & patrol location
	EIdleBehavior GetIdleBehavior() const { return IdleBehavior; }
	const TArray<AActor*>& GetPatrolTargets() const { return PatrolPoints; }
	float GetArrivalRadius() const { return ArrivalRadius; }
	
	// monster speed
	float GetCurrentSpeed() const { return BaseSpeed * SpeedMultiplier; }
	void SetSpeedMultiplier(float NewMultiplier) { SpeedMultiplier = NewMultiplier; }
    
	// monster sensing
	float GetBaseDetectionRange() const { return BaseDetectionRange; }
	float GetViewAngle() const { return ViewAngle; }
	float GetBaseHearingRange() const { return BaseHearingRange; }
	
	//monster type
	EMonsterType GetMonsterType() const { return MonsterType; }

	//monster attck
	float GetAttackRange() const { return AttackRange; }
	
	//monster chase
	float GetChaseRange() const { return ChaseRange; }
private:
	//일단 기본값은 시야기반
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	EMonsterType MonsterType = EMonsterType::Visual;
	
	//default = static(don't patrol)
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	EIdleBehavior IdleBehavior =EIdleBehavior::Static;
	
	
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	TArray<AActor*> PatrolPoints;
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	float ArrivalRadius = 50.f;
	
	UPROPERTY(EditAnywhere, Category = "Monster Stat")
	float BaseSpeed = 450.f;

	UPROPERTY(VisibleAnywhere, Category = "Monster Stat")
	float SpeedMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Monster Stat")
	float BaseDetectionRange = 1200.f;
	
	UPROPERTY(EditAnywhere, Category = "Monster Stat")
	float ViewAngle = 90.f;
	
	UPROPERTY(EditAnywhere, Category = "Monster Stat")
	float BaseHearingRange = 1500.f;
	
	UPROPERTY(EditAnywhere, Category = "Monster Stat")
	float AttackRange = 150.f;
	
	UPROPERTY(EditAnywhere, Category = "Monster Stat")
	float ChaseRange = 2000.f;
};