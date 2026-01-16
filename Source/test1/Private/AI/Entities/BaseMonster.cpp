#include "AI/Entities//BaseMonster.h"
#include "Global/BackRoomTags.h"
#include "GameplayTagContainer.h"
#include "AI/Components/MonsterStatusComponent.h"
#include "AI/Components/MonsterFSMComponent.h"
#include "AI/Components/MonsterSensingComponent.h"
#include "AI/Data/MonsterDataAsset.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"


ABaseMonster::ABaseMonster(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	// creat component
	StatusComponent  = CreateDefaultSubobject<UMonsterStatusComponent>(TEXT("StatusComponent"));
	FSMComponent     = CreateDefaultSubobject<UMonsterFSMComponent>(TEXT("FSMComponent"));
	SensingComponent = CreateDefaultSubobject<UMonsterSensingComponent>(TEXT("SensingComponent"));
	AudioLoopComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioLoopComponent"));
	AudioLoopComponent->SetupAttachment(GetRootComponent());
	AudioLoopComponent->SetAutoActivate(false);
	
	// visualize capsule comp
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetHiddenInGame(false);
	}
}

void ABaseMonster::FinishSpecialAbility_Implementation()
{
	if (FSMComponent)
	{
		FSMComponent->FinishSpecial();
	}
}

bool ABaseMonster::CanActivateSpecial_Implementation()
{
	return false;
}

bool ABaseMonster::OnPatrolWaitStart_Implementation()
{
	return false;
}

void ABaseMonster::OnLightHit() const
{
	
	FGameplayTag LightTag = FBackRoomTags::Get().Weakness_Light;
	if (MonsterData && MonsterData->WeaknessTags.HasTag(LightTag))
	{
		if (FSMComponent)
		{
			FSMComponent->ActivateStunState();
		}
	}
}

void ABaseMonster::SetState(EMonsterState NewState)
{
	if (FSMComponent)
	{
		FSMComponent->SetState(NewState);
	}
}

EMonsterState ABaseMonster::GetState() const
{
	if (FSMComponent)
	{
		return FSMComponent->GetCurrentState();
	}
	return EMonsterState::Idle;
}

void ABaseMonster::RefreshMonster()
{
	OnConstruction(GetActorTransform());
	
	UE_LOG(LogTemp, Log, TEXT("Monster Refreshed!"));
}

float ABaseMonster::GetBaseSpeed()
{
	if (StatusComponent)
	{
		return StatusComponent->GetBaseSpeed();
	}else
	{
		return 0.f;
	}
}

void ABaseMonster::SetBaseSpeed(float NewSpeed)
{
	if (StatusComponent)
	{
		StatusComponent->SetBaseSpeed(NewSpeed);
	}
}

float ABaseMonster::GetChaseSpeed()
{
	if (StatusComponent)
	{
		return StatusComponent->GetChaseSpeed();
	}else
	{
		return 0.f;
	}
}

void ABaseMonster::SetChaseSpeed(float NewSpeed)
{
	if (StatusComponent)
	{
		StatusComponent->SetChaseSpeed(NewSpeed);
	}
}
#if WITH_EDITOR
void ABaseMonster::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(ABaseMonster, MonsterData))
	{
		
		OnConstruction(GetActorTransform());
	}
}
#endif

void ABaseMonster::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (MonsterData)
	{
		
		if (MonsterData->MonsterMesh)
		{
			GetMesh()->SetSkeletalMesh(MonsterData->MonsterMesh);
		}
        
		
		if (MonsterData->AnimBPClass)
		{
			GetMesh()->SetAnimInstanceClass(MonsterData->AnimBPClass);
		}

		
		GetMesh()->SetRelativeScale3D(MonsterData->MeshScale);
        
		
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = MonsterData->BaseSpeed;
		}
	}
}

void ABaseMonster::BeginPlay()
{
	Super::BeginPlay();
	if (StatusComponent && MonsterData)
	{
		StatusComponent->InitData(MonsterData);
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = StatusComponent->GetBaseSpeed();

		}
	}
}



