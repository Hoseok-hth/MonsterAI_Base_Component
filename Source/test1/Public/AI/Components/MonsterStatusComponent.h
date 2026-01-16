#pragma once

#include "CoreMinimal.h"
#include "AI/Type/EMonsterState.h"
#include "AI/Navigation/MonsterPatrolPoint.h"
#include "Components/ActorComponent.h"
#include "MonsterStatusComponent.generated.h"

class UMonsterDataAsset;
class ABaseMonster;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEST1_API UMonsterStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UMonsterStatusComponent();
	virtual void BeginPlay() override;
	void InitData(const UMonsterDataAsset* BaseData);
	
	//monster idle type & patrol location
	//몬스터가 idle일때 행동 분류 & patrol Actor array 반환함수
	EIdleBehavior GetIdleBehavior() const { return IdleBehavior; }
	const TArray<AMonsterPatrolPoint*>& GetPatrolTargets() const { return PatrolPoints; }
	
	bool GetIsExecutionActive() const { return bIsExecutionActive; }
	void SetIsExecutionActive(bool b){bIsExecutionActive = b;}
	float GetBaseSpeed() const;
	void SetBaseSpeed(float speed);
	float GetChaseSpeed() const;
	void SetChaseSpeed(float speed);
	EMonsterType GetMonsterType() const;
	float GetArrivalRadius() const;
	float GetBaseDetectionRange() const;
	float GetViewAngle() const;
	float GetBaseHearingRange() const;
	float GetAttackRange() const;
	float GetChaseRange() const;
	float GetMenaceTime() const;
	float GetEyeHeight() const;
	float GetMaxLostTargetTime() const;
	float GetIdleSoundVolume() const;
	float GetChaseSoundVolume() const;
	bool GetIsSpecialActive() const;
	void SetIsSpecialActive(bool b);

	bool GetIsWaiting() const;
	void SetIsWaiting(bool b);
	float GetStunTime() const;
	
	
private:
	UPROPERTY()
	ABaseMonster* OwnerMonster;
	UPROPERTY()
	const UMonsterDataAsset* MonsterData;
	//default = static(don't patrol)
	//idle 기본 행동은 patrol없이 가만 멈춰있음
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	EIdleBehavior IdleBehavior =EIdleBehavior::Static;
	
	bool bIsExecutionActive = false;
	
	//patrol location points(actor)
	UPROPERTY(EditAnywhere, Category = "Monster Status | Patrol")
	TArray<AMonsterPatrolPoint*> PatrolPoints;
	
	
	UPROPERTY()
	float BaseSpeed = 0.f;
	UPROPERTY()
	float ChaseSpeed = 0.f;
	UPROPERTY()
	bool bIsSpecialActivate = false;
	UPROPERTY()
	bool bIsStunned = false;
	UPROPERTY()
	bool bIsWaiting = false;
	
	
};
