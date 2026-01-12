#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/Type/EMonsterState.h" // 1단계에서 만든 열거형
#include "MonsterFSMComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEST1_API UMonsterFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMonsterFSMComponent();

	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	void SetState(EMonsterState NewState);

protected:
	// 행동 패턴 핸들링 함수는 전부 가상화 해서 몬스터 타입에 맞게 상속받아서 새로 작성해야함
	virtual void BeginPlay() override;
	virtual void HandleIdle();
	virtual void HandleChase();
	virtual void HandleAttack();
	virtual void HandleMenace();
	virtual void Patrol();
	void UpdateMovementSpeed(EMonsterState NewState);
	UPROPERTY(VisibleAnywhere, Category = "AI")
	AActor* TargetActor;
	
	int32 CurrentPatrolIndex = 0;
	void UpdateNearestPatrolIndex();
	
private:
	UPROPERTY(VisibleAnywhere, Category = "AI")
	EMonsterState CurrentState = EMonsterState::None;
	UPROPERTY()
	class ABaseMonster* OwnerMonster;

	float DetectionTimer = 0.0f;
	float DetectionInterval = 0.2f;
	float MenaceTimer = 0.0f;
};