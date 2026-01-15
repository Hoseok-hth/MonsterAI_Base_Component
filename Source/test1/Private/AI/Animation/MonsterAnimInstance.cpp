// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Animation/MonsterAnimInstance.h"

#include "AI/Components/MonsterFSMComponent.h"
#include "AI/Entities/BaseMonster.h"

void UMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	auto Pawn = TryGetPawnOwner();
	if (!Pawn)
	{
		return;
	}
	CurrentSpeed = Pawn->GetVelocity().Size();
	auto Monster = Cast<ABaseMonster>(Pawn);
	if (!Monster)
	{
		return;
	}
	auto FSMComp = Monster->GetFSMComp();
	if (!FSMComp)
	{
		return;
	}
	State = FSMComp->GetCurrentState();
	bIsStunned = (State == EMonsterState::Stunned);
}
