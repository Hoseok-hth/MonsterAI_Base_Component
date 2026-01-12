#include "AI/Components/MonsterFSMComponent.h"

#include "AIController.h"
#include "GameFramework/Actor.h"
#include "AI/Components/MonsterSensingComponent.h"
#include "AI/Components/MonsterStatusComponent.h"
#include "AI/Entities/BaseMonster.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "Global/Define.h"

class AAIController;

UMonsterFSMComponent::UMonsterFSMComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;
}

void UMonsterFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	
	OwnerMonster = Cast<ABaseMonster>(GetOwner());
	SetState(EMonsterState::Idle);
}


// 틱으로 상태 업데이트
void UMonsterFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	switch (CurrentState)
	{
	case EMonsterState::Idle:
		HandleIdle();
		break;
	case EMonsterState::Chase:
		HandleChase();
		break;
	case EMonsterState::Attack:
		HandleAttack();
		break;
	default:
		break;
	}
}

void UMonsterFSMComponent::SetState(EMonsterState NewState)
{
	if (CurrentState == NewState) return;

	
	if (NewState == EMonsterState::Idle)
	{
		UpdateNearestPatrolIndex();
	}
	CurrentState = NewState;

	//log check
	UE_LOG(LogTemp, Warning, TEXT("==== [%s] State Changed to: %s ===="), 
		*GetOwner()->GetName(), 
		*UEnum::GetValueAsString(NewState));
}

// 기본 아이들일때
void UMonsterFSMComponent::HandleIdle()
{
	if (!OwnerMonster)
	{
		return;
	}
	//if DetectionTime less then Interval -> don't find Player
	//0.2초마다 플레이어 탐색, 최적화를 위해 있는 부분
	DetectionTimer += GetWorld() ->GetDeltaSeconds();
	if (DetectionTimer < DetectionInterval)
	{
		Patrol();
		return;
	}
	DetectionTimer = 0.0f;
	
	UMonsterSensingComponent* Sensing = OwnerMonster->FindComponentByClass<UMonsterSensingComponent>();
	UMonsterStatusComponent* Status = OwnerMonster->FindComponentByClass<UMonsterStatusComponent>();
	
	if (!Sensing || !Status)
	{
		return;
	}
	
	EMonsterType Type = Status->GetMonsterType();
	//trigger check, specific behavior defined child component
	//트리거 발생 우선 확인, 트리거시 행동은 자식컴포넌트에서 정의
	if (Sensing->GetIsTriggerd())
	{
		SetState(EMonsterState::Chase);
		return;
	}
	
	//Using StatusComponent Data, Set Searching Player Range(for performance)
	//StatusComponent 에 저장된 정보를 활용해서 플레이어 발견범위 제한(최적화)
	FVector MyLocation = OwnerMonster->GetActorLocation();
	float DetectionRadius =0.f;
	if (Status->GetMonsterType() == EMonsterType::Visual)
	{
		DetectionRadius = Status->GetBaseDetectionRange();
	}else if (Status->GetMonsterType() == EMonsterType::Acoustic)
	{
		DetectionRadius = Status->GetBaseHearingRange();
	}else if (Status->GetMonsterType() == EMonsterType::Trigger || Status->GetMonsterType() == EMonsterType::Hybrid)
	{
		if (Status->GetBaseHearingRange() < Status->GetBaseDetectionRange())
		{
			DetectionRadius = Status->GetBaseDetectionRange();
		}else
		{
			DetectionRadius = Status->GetBaseHearingRange();
		}
	}
	//find player in range
	//범위내 있는 대상이 플레이어인지 찾음
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
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			AActor* OverlappedActor = OverlapResult.GetActor();
			//if exist player in range(by "Player" Tag)
			//플레이어가 존재한 경우(player Tag 기반)
			if (OverlappedActor && OverlappedActor->ActorHasTag(FProjectTags::Player))
			{
				//is monster find? Check by MonsyerType
				//몬스터가 플레이어를 인식했는지를 몬스터타입별로 확인
				if (Type == EMonsterType::Visual || Type == EMonsterType::Hybrid)
				{
					if (Sensing->CanSeeTarget(OverlappedActor))
					{
						TargetActor = OverlappedActor;
						SetState(EMonsterState::Chase);
						return;
					}
			
				}
				if (Type == EMonsterType::Acoustic || Type == EMonsterType::Hybrid)
				{
					if (Sensing->WasSoundHeard())
					{
						TargetActor = OverlappedActor;
						SetState(EMonsterState::Chase);
						return;
					}
				}
			}
		}
	}	
	Patrol();
	
}

void UMonsterFSMComponent::HandleChase()
{
	UMonsterStatusComponent* Status = OwnerMonster->FindComponentByClass<UMonsterStatusComponent>();

	if (!OwnerMonster || !TargetActor)
	{
		SetState(EMonsterState::Idle);
		return;
	}
	AAIController* AIC = Cast<AAIController>(OwnerMonster->GetController());
	if (AIC)
	{
		AIC->MoveToActor(TargetActor,50.0f);
		float Distance = OwnerMonster->GetDistanceTo(TargetActor);
		if (Distance <= Status->GetAttackRange())
		{
			SetState(EMonsterState::Attack);
		}
		if (Distance > Status->GetChaseRange())
		{
			TargetActor = nullptr;
			AIC->StopMovement();
			SetState(EMonsterState::Idle);
		}
	}
}

void UMonsterFSMComponent::HandleAttack()
{
	
}

void UMonsterFSMComponent::Patrol()
{
	UMonsterStatusComponent* Status = OwnerMonster->FindComponentByClass<UMonsterStatusComponent>();

	if (Status->GetIdleBehavior() == EIdleBehavior::Patrol)
	{
		const TArray<AActor*>& Targets = Status->GetPatrolTargets();
		if (Targets.Num() == 0 || !Targets.IsValidIndex(CurrentPatrolIndex) || !Targets[CurrentPatrolIndex])
		{
			return;
		}
		AAIController* AIC = Cast<AAIController>(OwnerMonster->GetController());
		if (AIC)
		{
			FVector TargetLocation = Targets[CurrentPatrolIndex]->GetActorLocation();
			float DistToPoint = FVector::Dist(OwnerMonster->GetActorLocation(), TargetLocation);
			
			if (DistToPoint <= Status->GetArrivalRadius())
			{
				CurrentPatrolIndex = (CurrentPatrolIndex + 1) % Targets.Num();
				
			}else
			{
				AIC->MoveToLocation(TargetLocation,Status->GetArrivalRadius());
			}
		}
	}
}

void UMonsterFSMComponent::UpdateNearestPatrolIndex()
{
	UMonsterStatusComponent* Status = OwnerMonster->FindComponentByClass<UMonsterStatusComponent>();
	if (!Status)
	{
		return;
	}
	
	const TArray<AActor*>& Targets = Status->GetPatrolTargets();
	if (Targets.Num() == 0)
	{
		return;
	}
	float NearestDistance = TNumericLimits<float>::Max();
	int32 NearestIndex = 0;
	FVector MyLocation = OwnerMonster->GetActorLocation();
	
	for (int32 i =0; i <Targets.Num(); ++i)
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
