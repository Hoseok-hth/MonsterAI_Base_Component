#include "AI/Components/MonsterFSMComponent.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "AI/Components/MonsterSensingComponent.h"
#include "AI/Components/MonsterStatusComponent.h"
#include "AI/Entities/BaseMonster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "AI/Data/MonsterDataAsset.h"
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
		MoveComp = OwnerMonster->GetCharacterMovement();
		MonsterData = OwnerMonster->MonsterData;
	}


	SetState(EMonsterState::Idle);
}


// 틱으로 상태 업데이트
void UMonsterFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwnerMonster)
	{
		return;
	}
	if (!OwnerMonster->HasAuthority())
	{
		return;
	}
	switch (CurrentState)
	{
	case EMonsterState::Idle: HandleIdle();
		break;
	case EMonsterState::EyeChase: HandleEyeChase();
		break;
	case EMonsterState::EarChase: HandleEarChase();
		break;
	case EMonsterState::Attack: HandleAttack();
		break;
	case EMonsterState::Menace: HandleMenace();
		break;
	case EMonsterState::Special: HandleSpecial();
		break;
	case EMonsterState::Stunned: HandleStunned();
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
	
	
	//trigger check, specific behavior defined child component
	//트리거 발생 우선 확인, 트리거시 행동은 자식컴포넌트에서 정의
	if (Sensing->GetIsTriggerd())
	{
		SetState(EMonsterState::Special);
		return;
	}

	
	EMonsterType Type = MonsterData->MonsterType;
	if (Type == EMonsterType::Visual || Type == EMonsterType::Hybrid)
	{
		AActor* VisibleTarget = Sensing->FindNearestPlayer();
		if (VisibleTarget)
		{
			TargetActor = VisibleTarget;
		}
		//if monster see player -> Set EyeChase state
		//몬스터를 발견한다면 시야추격 모드
		if (TargetActor)
		{
			SetState(EMonsterState::EyeChase);
			return;
		}
		
	}
	//if monster heard noise -> Set EarChase State
	//몬스터가 소리를 들었다면 -> EarChase State 설정
	if (Type == EMonsterType::Acoustic || Type == EMonsterType::Hybrid)
	{
		if (Sensing->WasSoundHeard())
		{
			SetState(EMonsterState::EarChase);
			return;
		}
	}
	


	Patrol();
}

void UMonsterFSMComponent::HandleEyeChase()
{
	//if monster still see player -> keep chasing
	//몬스터가 타겟을 볼수 있다면 계속 추적
	if (Sensing->CanSeeTarget(TargetActor))
	{
		LostTargetTimer = 0.0f;
		AIC->MoveToActor(TargetActor, 30.0f);
	}
	//if monster can't see player -> keep chasing until MaxLostTargetTime
	//몬스터가 플레이어를 시야에서 놓치더라도 MaxLostTargetTime 만큼은 추격 유지
	else
	{
		LostTargetTimer += GetWorld()->GetDeltaSeconds();
		AIC->MoveToActor(TargetActor, 30.0f);
		if (LostTargetTimer >= MonsterData->MaxLostTargetTime)
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
	
	//if monster type is Hybrid, check monster can see any player. And chasing that player
	//만약 몬스터가 hybrid type 이라면, 시야에 플레이어를 포착했는지 확인. 이후 시야에 포착된 플레이어 우선 추격
	if (MonsterData->MonsterType == EMonsterType::Hybrid)
	{
		AActor* VisibleTarget = Sensing->FindNearestPlayer();
		if (VisibleTarget)
		{
			TargetActor = VisibleTarget;
			SetState(EMonsterState::EyeChase);
			return;
		}
	}
	
	//if monster arrive sound location, waiting until MaxLostTargetTime
	//if monster heard any sound when monster chasing or waiting, monster change arrival location.
	//목적지(소리 근원지)에 도착후, MaxLostTargetTime까지 도착지에서 대기
	//만약 추격도중 또는 대기도중 소리를 들었다면 목적지를 새로운 소리 근원지로 변경
	float DistToSound = FVector::Dist(OwnerMonster->GetActorLocation(),Sensing->GetLastSoundLocation());
	if (DistToSound <= MonsterData->ArrivalRadius + 30.f)
	{
		LostTargetTimer += GetWorld()->GetDeltaSeconds();
		if (LostTargetTimer >= MonsterData->ArrivalRadius)
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



void UMonsterFSMComponent::CheckCommonChaseTransition()
{
	if (!OwnerMonster || !Status) return;
	
	// if monster have Target and chasing, update Distance to target
	// 현재 추격 중인 타겟이 있다면 거리 갱신
	float Distance = TargetActor ? OwnerMonster->GetDistanceTo(TargetActor) : TNumericLimits<float>::Max();
    
	//check other player who is more close.
	// 시야 내 더 가까운 플레이어가 있는지 확인
	AActor* VisibleTarget = Sensing->FindNearestPlayer();
	if (VisibleTarget)
	{
		float DistToVisible = OwnerMonster->GetDistanceTo(VisibleTarget);
        
		
		if (!TargetActor || DistToVisible < Distance)
		{
			TargetActor = VisibleTarget;
			Distance = DistToVisible;
		}
	}

	if (!TargetActor) return;
	
	// if player is near than attack range, set state Attack
	// 플레이어가 공격 사거리 안에 들어왔다면 공격
	if (Distance <= MonsterData->AttackRange)
	{
		SetState(EMonsterState::Attack);
		ExecuteKill(TargetActor);
		return;
	}

	//if player further than chase range, stop chase.
	// 추격 범위 이탈 체크
	if (Distance > MonsterData->AttackRange)
	{
		StopChasing();
	}
}

void UMonsterFSMComponent::StopChasing()
{
	TargetActor = nullptr;
	if (Sensing)
	{
		Sensing->ResetSoundFlag();
	}
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
		//set monster front to target
		// 몬스터가 공격대상을 바라보게 설정
		FVector LookDir = TargetActor->GetActorLocation() - OwnerMonster->GetActorLocation();
		LookDir.Z = 0.f;
		if (!LookDir.IsNearlyZero())
		{
			OwnerMonster->SetActorRotation(LookDir.Rotation());
		}
	}
	
	//if finish execution(ex execution animation montage), Change state Menace.
	// 처형이 종료되면, Menace State로 변경
	if (Status && !Status->GetIsExecutionActive())
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
	if (MenaceTimer >= MonsterData->MenaceDuration)
	{
		SetState(EMonsterState::Idle);
		
	}
}

void UMonsterFSMComponent::HandleSpecial()
{
}

void UMonsterFSMComponent::HandleStunned()
{
}

void UMonsterFSMComponent::Patrol()
{
	if (!Status || !OwnerMonster || !AIC)
	{
		return;
	}
	//monster stay waiting point, until PatrolTargetWaitTime
	//대기 지점에서는 몬스터는 PatrolTargetWaitTime만큼 대기한다.
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
		
		//if monster arrive target point, check that point have waiting time
		//패트롤 목적지에 도착하면, 해당 지점에 대기시간이 있는지 확인한다.
		FVector MyLocation = OwnerMonster->GetActorLocation();
		FVector TargetLocation = Targets[CurrentPatrolIndex]->GetActorLocation();
		float DistToPoint = FVector::Dist2D(MyLocation, TargetLocation);
		if (DistToPoint <= MonsterData->ArrivalRadius + 30.f)
		{
			if (Targets[CurrentPatrolIndex]->WaitTime > 0.0f)
			{
				bIsWaiting = true;
				PatrolTargetWaitTime = Targets[CurrentPatrolIndex]->WaitTime;
				AIC->StopMovement();
				UE_LOG(LogTemp, Warning, TEXT("지점 도달: %f초간 대기 시작"), PatrolTargetWaitTime);
			}
			//if there is no waiting time, monster go to next patrol point
			//만약 waiting time이 존재하지 않는다면, 몬스터는 다음 패트롤 지점으로 이동한다.
			else
			{
				CurrentPatrolIndex = (CurrentPatrolIndex + 1) % Targets.Num();
				TargetLocation = Targets[CurrentPatrolIndex]->GetActorLocation();
				UE_LOG(LogTemp, Warning, TEXT("목적지 도달! 다음 인덱스로 변경: %d"), CurrentPatrolIndex);
			}
		}

		//when monster Can't arrive target point, try go to other location near the target
		//만약 목적지로 가는 길을 찾지 못할경우, 목적지 근처라도 가게 설정
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
	MenaceTimer = 0.0f;
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
	if (!OwnerMonster || !MoveComp || !Status) return;

	switch (NewState)
	{
	case EMonsterState::Idle:
		MoveComp->MaxWalkSpeed = MonsterData->BaseSpeed;
		break;
	case EMonsterState::EyeChase:
	case EMonsterState::EarChase:
	case EMonsterState::HybridChase:
		MoveComp->MaxWalkSpeed = MonsterData->ChaseSpeed;
		break;
	case EMonsterState::Attack:
	case EMonsterState::Menace:
		MoveComp->MaxWalkSpeed = 0.0f;
		break;
	default:
		break;
	}
}

void UMonsterFSMComponent::ExecuteKill(AActor* Victim)
{
	if (!Victim || !OwnerMonster)
	{
		return;
	}
	//Check Target have ExecutionTargetInterface
	//타켓이 ExecutionTargetInterface를 구현했는지 확인
	IExecutionTargetInterface* Target = Cast<IExecutionTargetInterface>(Victim);
	if (!Target)
	{
		return;
	}
	//Call Target, who is Killed
	//타겟에게 죽었음을 알림
	Target->OnCaughtByMonster(OwnerMonster);
	
	//Play Execution anymation montage logic
	//처형 애니메이션 몽타쥬 재생 로직
	UAnimMontage* AnimExecution = MonsterData->ExecutionMontage;
	if (!AnimExecution)
	{
		return;
	}
	UAnimInstance* AnimInstance = OwnerMonster->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		float Duration = OwnerMonster->PlayAnimMontage(AnimExecution);
		if (Duration > 0.0f)
		{
			Status->SetIsExecutionActive(true);
			FOnMontageEnded MontageEndDelegate;
			MontageEndDelegate.BindUObject(this,&UMonsterFSMComponent::OnExecutionMontageEnded);
			AnimInstance->Montage_SetEndDelegate(MontageEndDelegate,AnimExecution);
		}
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

void UMonsterFSMComponent::OnExecutionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (MonsterData && Montage == MonsterData->ExecutionMontage)
	{
		Status->SetIsExecutionActive(false);
	}
}
