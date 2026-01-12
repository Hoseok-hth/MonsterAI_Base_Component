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

protected:
	virtual void BeginPlay() override;

	// components 스테이터스, state , sensing, combat, + movement 까지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Component")
	class UMonsterStatusComponent* StatusComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Component")
	class UMonsterFSMComponent* FSMComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Component")
	class UMonsterSensingComponent* SensingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Component")
	class UMonsterCombatComponent* CombatComponent;
};