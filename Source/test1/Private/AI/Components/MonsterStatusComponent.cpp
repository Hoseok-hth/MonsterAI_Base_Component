#include "AI/Components/MonsterStatusComponent.h"
#include "AI/Data/MonsterDataAsset.h"
#include "AI/Entities/BaseMonster.h"

UMonsterStatusComponent::UMonsterStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMonsterStatusComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UMonsterStatusComponent::InitData(const UMonsterDataAsset* BaseData)
{
	if (BaseData)
	{
		MonsterData = BaseData;
		BaseSpeed = BaseData->BaseSpeed;
		ChaseSpeed = BaseData->ChaseSpeed;
	}
}

float UMonsterStatusComponent::GetBaseSpeed() const
{
	return BaseSpeed;
}

void UMonsterStatusComponent::SetBaseSpeed(float speed)
{
	BaseSpeed = speed;
}

float UMonsterStatusComponent::GetChaseSpeed() const
{
	return ChaseSpeed;
}

void UMonsterStatusComponent::SetChaseSpeed(float speed)
{
	ChaseSpeed = speed;
}

EMonsterType UMonsterStatusComponent::GetMonsterType() const
{
	return MonsterData->MonsterType;
}

float UMonsterStatusComponent::GetArrivalRadius() const
{
	return MonsterData->ArrivalRadius;
}

float UMonsterStatusComponent::GetBaseDetectionRange() const
{
	return MonsterData->BaseDetectionRange;
}

float UMonsterStatusComponent::GetViewAngle() const
{
	return MonsterData->ViewAngle;
}

float UMonsterStatusComponent::GetBaseHearingRange() const
{
	return MonsterData->BaseHearingRange;
}

float UMonsterStatusComponent::GetAttackRange() const
{
	return MonsterData->AttackRange;
}

float UMonsterStatusComponent::GetChaseRange() const
{
	return MonsterData->ChaseRange;
}

float UMonsterStatusComponent::GetMenaceTime() const
{
	return MonsterData->MenaceTime;
}

float UMonsterStatusComponent::GetEyeHeight() const
{
	return MonsterData->EyeHeight;
}

float UMonsterStatusComponent::GetMaxLostTargetTime() const
{
	return MonsterData->MaxLostTargetTime;
}

float UMonsterStatusComponent::GetIdleSoundVolume() const
{
	return MonsterData->IdleSoundVolume;
}

float UMonsterStatusComponent::GetChaseSoundVolume() const
{
	return MonsterData->ChaseSoundVolume;
}

bool UMonsterStatusComponent::GetIsSpecialActive() const
{
	return bIsSpecialActivate;
}

void UMonsterStatusComponent::SetIsSpecialActive(bool b)
{
	bIsSpecialActivate = b;
}

bool UMonsterStatusComponent::GetIsWaiting() const
{
	return bIsWaiting;
}

void UMonsterStatusComponent::SetIsWaiting(bool b)
{
	bIsWaiting = b;
}

float UMonsterStatusComponent::GetStunTime() const
{
	return MonsterData->StunnedTime;
}
