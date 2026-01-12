#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseMonster.generated.h"

UCLASS()
class TEST1_API ABaseMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseMonster(const FObjectInitializer& ObjectInitializer);
	void ExecuteKill(AActor* Victim);
	bool IsExecutionFinished() const{return !bIsExecutionActive;}
	void FinishExecution(){bIsExecutionActive = false;}
	

protected:
	virtual void BeginPlay() override;

	// components 스테이터스, state , sensing, combat,
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Component")
	class UMonsterStatusComponent* StatusComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Component")
	class UMonsterFSMComponent* FSMComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Component")
	class UMonsterSensingComponent* SensingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Component")
	class UMonsterCombatComponent* CombatComponent;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Animation")
	class UAnimMontage* ExecutionMontage;
	
private:
	bool bIsExecutionActive = false;
};