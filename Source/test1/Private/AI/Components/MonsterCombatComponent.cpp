#include "AI/Components/MonsterCombatComponent.h"

UMonsterCombatComponent::UMonsterCombatComponent() { PrimaryComponentTick.bCanEverTick = false; }

void UMonsterCombatComponent::ProcessAttack(AActor* Target) { }

bool UMonsterCombatComponent::CheckHit(float AttackRadius) { return false; }