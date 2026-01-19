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
#include "DrawDebugHelpers.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"


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

void ABaseMonster::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
#if WITH_EDITOR
	if (MonsterData && StatusComponent)
	{
		FlushPersistentDebugLines(GetWorld());

		FVector Center = GetActorLocation();
		Center.Z += MonsterData->EyeHeight;

		float View_Range = MonsterData->BaseDetectionRange;
		float View_Angle = MonsterData->ViewAngle;
		float Hearing_Range = MonsterData->BaseHearingRange;

		FVector Forward = GetActorForwardVector();
		FVector Right = GetActorRightVector();
		FVector Up = GetActorUpVector();

		// [수정됨] 원을 바닥에 눕히기 위해 Matrix의 첫 번째 인자(X축/Normal)를 Up 벡터로 변경
		// 순서: (Normal, YAxis, ZAxis, Origin)
		// Up 벡터가 Normal이 되면 원은 수평면(Forward-Right 평면)에 그려집니다.
		DrawDebugCircle(
			GetWorld(), 
			FMatrix(Up, Right, Forward, Center), 
			View_Range, 
			32, 
			FColor::Green, 
			true, 
			-1.f, 
			0, 
			2.0f, 
			false 
		);
		
		DrawDebugCircle(
			GetWorld(), 
			FMatrix(Up, Right, Forward, Center), 
			Hearing_Range, 
			32, 
			FColor::Red, 
			true, 
			-1.f, 
			0, 
			2.0f, 
			false 
		);

		// 시야각(부채꼴) 선 그리기 (이 부분은 기존과 동일)
		FVector LeftDir = Forward.RotateAngleAxis(-View_Angle * 0.5f, FVector::UpVector);
		FVector RightDir = Forward.RotateAngleAxis(View_Angle * 0.5f, FVector::UpVector);

		DrawDebugLine(GetWorld(), Center, Center + LeftDir * View_Range, FColor::Green, true, -1.f, 0, 2.0f);
		DrawDebugLine(GetWorld(), Center, Center + RightDir * View_Range, FColor::Green, true, -1.f, 0, 2.0f);
	}
#endif
	
}

void ABaseMonster::FinishSpecialAbility_Implementation()
{
	if (FSMComponent)
	{
		FSMComponent->FinishSpecial();
	}
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
		
		if (GUnrealEd)
		{
			GUnrealEd->RedrawLevelEditingViewports();
		}
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
	
#if WITH_EDITOR
	if (MonsterData && StatusComponent)
	{
		FlushPersistentDebugLines(GetWorld());

		FVector Center = GetActorLocation();
		Center.Z += MonsterData->EyeHeight;

		float View_Range = MonsterData->BaseDetectionRange;
		float View_Angle = MonsterData->ViewAngle;
		float Hearing_Range = MonsterData->BaseHearingRange;

		FVector Forward = GetActorForwardVector();
		FVector Right = GetActorRightVector();
		FVector Up = GetActorUpVector();

		// [수정됨] 원을 바닥에 눕히기 위해 Matrix의 첫 번째 인자(X축/Normal)를 Up 벡터로 변경
		// 순서: (Normal, YAxis, ZAxis, Origin)
		// Up 벡터가 Normal이 되면 원은 수평면(Forward-Right 평면)에 그려집니다.
		DrawDebugCircle(
			GetWorld(), 
			FMatrix(Up, Right, Forward, Center), 
			View_Range, 
			32, 
			FColor::Green, 
			true, 
			-1.f, 
			0, 
			2.0f, 
			false 
		);
		
		DrawDebugCircle(
			GetWorld(), 
			FMatrix(Up, Right, Forward, Center), 
			Hearing_Range, 
			32, 
			FColor::Red, 
			true, 
			-1.f, 
			0, 
			2.0f, 
			false 
		);

		// 시야각(부채꼴) 선 그리기 (이 부분은 기존과 동일)
		FVector LeftDir = Forward.RotateAngleAxis(-View_Angle * 0.5f, FVector::UpVector);
		FVector RightDir = Forward.RotateAngleAxis(View_Angle * 0.5f, FVector::UpVector);

		DrawDebugLine(GetWorld(), Center, Center + LeftDir * View_Range, FColor::Green, true, -1.f, 0, 2.0f);
		DrawDebugLine(GetWorld(), Center, Center + RightDir * View_Range, FColor::Green, true, -1.f, 0, 2.0f);
	}
#endif
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



