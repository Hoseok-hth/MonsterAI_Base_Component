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
	
	bool GetIsExecutionActive() const { return bIsExecutionActive; }
	void SetIsExecutionActive(bool b){bIsExecutionActive = b;}
private:
	//default = visual sense
	//기본값은 시야기반 감지
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	EMonsterType MonsterType = EMonsterType::Visual;
	
	//default = static(don't patrol)
	//idle 기본 행동은 patrol없이 가만 멈춰있음
	UPROPERTY(EditAnywhere, Category = "Monster Status")
	EIdleBehavior IdleBehavior =EIdleBehavior::Static;
	
	bool bIsExecutionActive = false;
	
	
	
	//patrol location points(actor)
	UPROPERTY(EditAnywhere, Category = "Monster Status | Patrol")
	TArray<AMonsterPatrolPoint*> PatrolPoints;
	
	
};