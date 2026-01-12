#include "AI/Entities//BaseMonster.h"
#include "AI/Components/MonsterStatusComponent.h"
#include "AI/Components/MonsterFSMComponent.h"
#include "AI/Components/MonsterSensingComponent.h"
#include "AI/Components/MonsterCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

ABaseMonster::ABaseMonster(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	// creat component
	StatusComponent  = CreateDefaultSubobject<UMonsterStatusComponent>(TEXT("StatusComponent"));
	FSMComponent     = CreateDefaultSubobject<UMonsterFSMComponent>(TEXT("FSMComponent"));
	SensingComponent = CreateDefaultSubobject<UMonsterSensingComponent>(TEXT("SensingComponent"));
	CombatComponent  = CreateDefaultSubobject<UMonsterCombatComponent>(TEXT("CombatComponent"));

	// visualize capsule comp
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetHiddenInGame(false);
	}
}

void ABaseMonster::BeginPlay()
{
	Super::BeginPlay();

	if (StatusComponent && GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = StatusComponent->GetCurrentSpeed();
	}
}