#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MonsterCombatComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEST1_API UMonsterCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMonsterCombatComponent();
	
	void ProcessAttack(AActor* Target);
	bool CheckHit(float AttackRadius);
};