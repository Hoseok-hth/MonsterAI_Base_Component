#include "AI/Components/MonsterFSMComponent.h"

#include "AIController.h"
#include "GameFramework/Actor.h"
#include "AI/Components/MonsterSensingComponent.h"
#include "AI/Components/MonsterStatusComponent.h"
#include "AI/Entities/BaseMonster.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "Global/Define.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "Interfaces/ExecutionTargetInterface.h"

class AAIController;

UMonsterFSMComponent::UMonsterFSMComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMonsterFSMComponent::BeginPlay()
{
	Super::BeginPlay();


	OwnerMonster = Cast<ABaseMonster>(GetOwner());
	if (OwnerMonster)
	{
		Status = OwnerMonster->FindComponentByClass<UMonsterStatusComponent>();
		Sensing = OwnerMonster->FindComponentByClass<UMonsterSensingComponent>();
		AIC = Cast<AAIController>(OwnerMonster->GetController());
	}


	SetState(EMonsterState::Idle);
}


// 틱으로 상태 업데이트
void UMonsterFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (CurrentState)
	{
	case EMonsterState::Idle: HandleIdle();
		break;
	case EMonsterState::EyeChase: HandleEyeChase();
		break;
	case EMonsterState::EarChase: HandleEarChase();
		break;
	case EMonsterState::HybridChase: HandleHybridChase();
		break;
	case EMonsterState::Attack: HandleAttack();
		break;
	case EMonsterState::Menace: HandleMenace();
		break;
	default: break;
	}
}

void UMonsterFSMComponent::SetState(EMonsterState NewState)
{
	if (CurrentState == NewState) return;
	ExitCurrentState();
	EMonsterState PreviousState = CurrentState;
	CurrentState = NewState;
	EnterNewState(PreviousState);

	//log check
	UE_LOG(LogTemp, Warning, TEXT("==== [%s] %s -> %s ===="),
	       *GetOwner()->GetName(),
	       *UEnum::GetValueAsString(PreviousState),
	       *UEnum::GetValueAsString(NewState));
}

// 기본 아이들일때
void UMonsterFSMComponent::HandleIdle()
{
	if (!OwnerMonster || !Sensing || !Status)
	{
		return;
	}
	//if DetectionTime less then Interval -> don't find Player
	//0.2초마다 플레이어 탐색, 최적화를 위해 있는 부분
	DetectionTimer += GetWorld()->GetDeltaSeconds();
	if (DetectionTimer < DetectionInterval)
	{
		Patrol();
		return;
	}
	DetectionTimer = 0.0f;


	EMonsterType Type = Status->GetMonsterType();
	//trigger check, specific behavior defined child component
	//트리거 발생 우선 확인, 트리거시 행동은 자식컴포넌트에서 정의
	if (Sensing->GetIsTriggerd())
	{
		SetState(EMonsterState::Special);
		return;
	}

	//Using StatusComponent Data, Set Searching Player Range(for performance)
	//StatusComponent 에 저장된 정보를 활용해서 플레이어 발견범위 제한(최적화)
	if (Type == EMonsterType::Visual || Type == EMonsterType::Hybrid)
	{
		FVector MyLocation = OwnerMonster->GetActorLocation();
		float DetectionRadius = Status->GetBaseDetectionRange();


		TArray<FOverlapResult> OverlapResults;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(OwnerMonster);

		bool bHit = GetWorld()->OverlapMultiByChannel(
			OverlapResults,
			MyLocation,
			FQuat::Identity,
			ECC_Pawn,
			FCollisionShape::MakeSphere(DetectionRadius),
			QueryParams
		);

		if (bHit)
		{
			float NearestDistance = TNumericLimits<float>::Max();

			for (const FOverlapResult& OverlapResult : OverlapResults)
			{
				AActor* OverlappedActor = OverlapResult.GetActor();
				//if exist player in range(by "Player" Tag)
				//플레이어가 존재한 경우(player Tag 기반)
				if (OverlappedActor && OverlappedActor->ActorHasTag(FProjectTags::Player))
				{
					if (Sensing->CanSeeTarget(OverlappedActor))
					{
						float Distance = OwnerMonster->GetDistanceTo(OverlappedActor);
						if (Distance <= NearestDistance)
						{
							NearestDistance = Distance;
							TargetActor = OverlappedActor;
						}
					}
				}
			}

			if (TargetActor)
			{
				SetState(EMonsterState::EyeChase);
				return;
			}
		}
		if (Type == EMonsterType::Acoustic || Type == EMonsterType::Hybrid)
		{
			if (Sensing->WasSoundHeard())
			{
				SetState(EMonsterState::EarChase);
				return;
			}
		}
	}


	Patrol();
}

void UMonsterFSMComponent::HandleEyeChase()
{
	if (Sensing->CanSeeTarget(TargetActor))
	{
		LostTargetTimer = 0.0f;
		AIC->MoveToActor(TargetActor, 30.0f);
	}
	else
	{
		LostTargetTimer += GetWorld()->GetDeltaSeconds();
		AIC->MoveToActor(TargetActor, 30.0f);
		if (LostTargetTimer >= Status->GetMaxLostTargetTime())
		{
			StopChasing();
			return;
		}
	}
	CheckCommonChaseTransition();
}

void UMonsterFSMComponent::HandleEarChase()
{
	if (!Sensing || !AIC)
	{
		StopChasing();
		return;
	}
	if (Status->GetMonsterType() == EMonsterType::Hybrid)
	{
		AActor* VisibleTarget = Sensing->FindNearestPlayer();
		if (VisibleTarget)
		{
			TargetActor = VisibleTarget;
			SetState(EMonsterState::EyeChase);
			return;
		}
	}
	float DistToSound = FVector::Dist(OwnerMonster->GetActorLocation(),Sensing->GetLastSoundLocation());
	if (DistToSound <= Status->GetArrivalRadius() + 30.f)
	{
		LostTargetTimer += GetWorld()->GetDeltaSeconds();
		if (LostTargetTimer >= Status->GetMaxLostTargetTime())
		{
			StopChasing();
			return;
		}
	}else
	{
		LostTargetTimer = 0.0f;
		AIC->MoveToLocation(Sensing->GetLastSoundLocation());
	}
	
	CheckCommonChaseTransition();
}

void UMonsterFSMComponent::HandleHybridChase()
{
	if (!TargetActor || !Sensing || !AIC)
	{
		StopChasing();
		return;
	}
	bool bCanSee = Sensing->CanSeeTarget(TargetActor);
	bool bCanHear = Sensing->WasSoundHeard();

	if (bCanSee || bCanHear)
	{
		LostTargetTimer = 0.0f;
		if (bCanSee)
		{
			AIC->MoveToActor(TargetActor, 30.0f);
		}
		else
		{
			AIC->MoveToLocation(Sensing->GetLastSoundLocation(), 30.0f);
		}
	}
	else
	{
		LostTargetTimer += GetWorld()->GetDeltaSeconds();
		if (LostTargetTimer >= Status->GetMaxLostTargetTime())
		{
			StopChasing();
			return;
		}
		AIC->MoveToLocation(TargetActor->GetActorLocation(), 30.0f);
	}
	CheckCommonChaseTransition();
}

void UMonsterFSMComponent::CheckCommonChaseTransition()
{
	if (!OwnerMonster || !Status) return;

	// 현재 추격 중인 타겟이 있다면 거리 갱신
	float Distance = TargetActor ? OwnerMonster->GetDistanceTo(TargetActor) : TNumericLimits<float>::Max();
    
	// 시야 내 더 가까운 플레이어가 있는지 확인
	AActor* VisibleTarget = Sensing->FindNearestPlayer();
	if (VisibleTarget)
	{
		float DistToVisible = OwnerMonster->GetDistanceTo(VisibleTarget);
        
		// 타겟이 없었거나, 새로 발견한 타겟이 더 가깝다면 교체
		if (!TargetActor || DistToVisible < Distance)
		{
			TargetActor = VisibleTarget;
			Distance = DistToVisible;
		}
	}

	if (!TargetActor) return;

	// 공격 범위 체크
	if (Distance <= Status->GetAttackRange())
	{
		SetState(EMonsterState::Attack);
		return;
	}

	// 추격 범위 이탈 체크
	if (Distance > Status->GetChaseRange())
	{
		StopChasing();
	}
}

void UMonsterFSMComponent::StopChasing()
{
	TargetActor = nullptr;
	if (AIC)
	{
		AIC->StopMovement();
		SetState(EMonsterState::Idle);
	}
}

void UMonsterFSMComponent::HandleAttack()
{
	if (AIC)
	{
		AIC->StopMovement();
	}
	if (TargetActor)
	{
		FVector LookDir = TargetActor->GetActorLocation() - OwnerMonster->GetActorLocation();
		LookDir.Z = 0.f;
		if (!LookDir.IsNearlyZero())
		{
			OwnerMonster->SetActorRotation(LookDir.Rotation());
		}
	}
	if (OwnerMonster && OwnerMonster->IsExecutionFinished())
	{
		SetState(EMonsterState::Menace);
	}
}

void UMonsterFSMComponent::HandleMenace()
{
	if (!Status)
	{
		return;
	}
	MenaceTimer += GetWorld()->GetDeltaSeconds();
	if (MenaceTimer >= Status->GetMenaceDuration())
	{
		SetState(EMonsterState::Idle);
		//TargetActor = nullptr;
	}
}

void UMonsterFSMComponent::Patrol()
{
	if (!Status || !OwnerMonster || !AIC)
	{
		return;
	}
	if (bIsWaiting)
	{
		PatrolWaitTimer += GetWorld()->GetDeltaSeconds();
		if (PatrolWaitTimer >= PatrolTargetWaitTime)
		{
			bIsWaiting = false;
			PatrolWaitTimer = 0.0f;
			const TArray<AMonsterPatrolPoint*>& Targets = Status->GetPatrolTargets();
			CurrentPatrolIndex = (CurrentPatrolIndex + 1) % Targets.Num();
		}
		return;
	}
	if (Status->GetIdleBehavior() == EIdleBehavior::Patrol)
	{
		const TArray<AMonsterPatrolPoint*>& Targets = Status->GetPatrolTargets();
		if (Targets.Num() == 0 || !Targets.IsValidIndex(CurrentPatrolIndex) || !Targets[CurrentPatrolIndex])
		{
			return;
		}
		FVector MyLocation = OwnerMonster->GetActorLocation();
		FVector TargetLocation = Targets[CurrentPatrolIndex]->GetActorLocation();
		float DistToPoint = FVector::Dist2D(MyLocation, TargetLocation);

		//UE_LOG(LogTemp, Warning, TEXT("목적지까지 남은 거리: %f | 판정 거리: %f"), DistToPoint, Status->GetArrivalRadius());
		if (DistToPoint <= Status->GetArrivalRadius() + 50.f)
		{
			if (Targets[CurrentPatrolIndex]->WaitTime > 0.0f)
			{
				bIsWaiting = true;
				PatrolTargetWaitTime = Targets[CurrentPatrolIndex]->WaitTime;
				AIC->StopMovement();
				//UE_LOG(LogTemp, Warning, TEXT("지점 도달: %f초간 대기 시작"), PatrolTargetWaitTime);
			}
			else
			{
				CurrentPatrolIndex = (CurrentPatrolIndex + 1) % Targets.Num();
				TargetLocation = Targets[CurrentPatrolIndex]->GetActorLocation();
				//UE_LOG(LogTemp, Warning, TEXT("목적지 도달! 다음 인덱스로 변경: %d"), CurrentPatrolIndex);
			}
		}

		if (AIC->GetMoveStatus() != EPathFollowingStatus::Moving)
		{
			UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			FNavLocation ProjectedLocation;
			if (NavSys && NavSys->ProjectPointToNavigation(TargetLocation, ProjectedLocation,
			                                               FVector(500.f, 500.f, 500.f)))
			{
				AIC->MoveToLocation(ProjectedLocation.Location, 30.f);
			}
		}
	}
}

void UMonsterFSMComponent::ExitCurrentState()
{
	bIsWaiting = false;
	PatrolWaitTimer = 0.0f;
	DetectionTimer = 0.0f;
	LostTargetTimer = 0.0f;
}

void UMonsterFSMComponent::EnterNewState(EMonsterState PreviousState)
{
	UpdateMovementSpeed(CurrentState);
	switch (CurrentState)
	{
	case EMonsterState::Idle:
		if (PreviousState != EMonsterState::Idle)
		{
			UpdateNearestPatrolIndex();
		}
		break;
	case EMonsterState::Menace:
		MenaceTimer = 0.0f;
		if (AIC) AIC->StopMovement();
		break;
	case EMonsterState::Attack:
		if (AIC) AIC->StopMovement();
		break;
	case EMonsterState::Special:
		break;
	case EMonsterState::Stunned:
		break;
	default:
		break;
	}
}

void UMonsterFSMComponent::UpdateMovementSpeed(EMonsterState NewState)
{
	if (!OwnerMonster) return;
	UCharacterMovementComponent* MoveComp = OwnerMonster->GetCharacterMovement();
	if (!MoveComp || !Status) return;

	switch (NewState)
	{
	case EMonsterState::Idle:
		MoveComp->MaxWalkSpeed = Status->GetBaseSpeed();
		break;
	case EMonsterState::EyeChase:
	case EMonsterState::EarChase:
	case EMonsterState::HybridChase:
		MoveComp->MaxWalkSpeed = Status->GetChaseSpeed();
		break;
	case EMonsterState::Attack:
	case EMonsterState::Menace:
		MoveComp->MaxWalkSpeed = 0.0f;
		break;
	default:
		break;
	}
}

void UMonsterFSMComponent::UpdateNearestPatrolIndex()
{
	if (!Status || !OwnerMonster)
	{
		return;
	}

	const TArray<AMonsterPatrolPoint*>& Targets = Status->GetPatrolTargets();
	if (Targets.Num() == 0)
	{
		return;
	}
	float NearestDistance = TNumericLimits<float>::Max();
	int32 NearestIndex = 0;
	FVector MyLocation = OwnerMonster->GetActorLocation();

	for (int32 i = 0; i < Targets.Num(); ++i)
	{
		if (Targets[i])
		{
			float Distance = FVector::Dist(MyLocation, Targets[i]->GetActorLocation());
			if (Distance < NearestDistance)
			{
				NearestDistance = Distance;
				NearestIndex = i;
			}
		}
	}
	CurrentPatrolIndex = NearestIndex;
}
