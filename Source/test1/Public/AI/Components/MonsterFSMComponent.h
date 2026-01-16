#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/Type/EMonsterState.h" 
#include "MonsterFSMComponent.generated.h"

class UMonsterDataAsset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEST1_API UMonsterFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMonsterFSMComponent();

	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	void SetState(EMonsterState NewState);
	EMonsterState GetCurrentState()const{return CurrentState;};
	UPROPERTY()
	const UMonsterDataAsset* MonsterData;
	void ActivateStunState();
	void FinishSpecial();
protected:
	// 행동 패턴 핸들링 함수는 전부 가상화 해서 몬스터 타입에 맞게 상속받아서 새로 작성해야함
	virtual void BeginPlay() override;
	virtual void HandleIdle();
	virtual void Patrol();
	
	virtual void HandleEyeChase();
	virtual void HandleEarChase();
	virtual void CheckCommonChaseTransition();
	virtual void StopChasing();
	
	virtual void HandleAttack();
	virtual void HandleMenace();
	
	virtual void HandleSpecial();
	virtual void HandleStunned();
	
	virtual void HandleFinishSpecial();
	
	virtual void ExitCurrentState();
	virtual void EnterNewState(EMonsterState PreviousState);
	void UpdateMovementSpeed(EMonsterState NewState);
	void UpdateLoopSound(EMonsterState NewState);
	
	UPROPERTY(VisibleAnywhere, Category = "AI")
	AActor* TargetActor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI | Component")
	class UMonsterStatusComponent* Status;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI | Component")
	class UMonsterSensingComponent* Sensing;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI | Component")
	class AAIController* AIC;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI | Component")
	class UCharacterMovementComponent* MoveComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI | Component")
	class UAudioComponent* AudioComp;
	 
	
	int32 CurrentPatrolIndex = 0;
	void UpdateNearestPatrolIndex();
	
	UFUNCTION()
	void OnRep_CurrentState();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentState, VisibleAnywhere, Category = "AI")
	EMonsterState CurrentState = EMonsterState::None;
	UPROPERTY()
	class ABaseMonster* OwnerMonster;

	void ExecuteKill(AActor* Victim);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayMontage(UAnimMontage* MontageToPlay);
	
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	float DetectionTimer = 0.0f;
	float DetectionInterval = 0.2f;
	float MenaceTimer = 0.0f;
	float PatrolWaitTimer = 0.0f;
	float PatrolTargetWaitTime = 0.0f;
	float LostTargetTimer = 0.0f;
	float StunTimer = 0.0f;
	
	
	
};