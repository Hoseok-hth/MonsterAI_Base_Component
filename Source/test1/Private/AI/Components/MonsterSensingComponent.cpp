#include "AI/Components/MonsterSensingComponent.h"
#include "AI/Entities/BaseMonster.h"
#include "AI/Components/MonsterStatusComponent.h"
#include "GameFramework/Character.h"
#include "Global/Define.h"
#include "Kismet/GameplayStatics.h"

void UMonsterSensingComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = Cast<ABaseMonster>(GetOwner());
}

UMonsterSensingComponent::UMonsterSensingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

bool UMonsterSensingComponent::CanSeeTarget(AActor* Target)
{
	if (!Target)
	{
		return false;
	}
    
	UMonsterStatusComponent* Status = Owner->FindComponentByClass<UMonsterStatusComponent>();
	if (!Status)
	{
		return false;
	}
	float MaxRange = Status->GetBaseDetectionRange();
	float fovAngle = Status->GetViewAngle();
    
	FVector MonsterLocation = GetOwner()->GetActorLocation();
	FVector EyeLocation = MonsterLocation + FVector(0,0,Status->GetEyeHeight());
	FVector TargetLocation = Target->GetActorLocation();
	float Distance = FVector::Dist(MonsterLocation, TargetLocation);
    
	if (Distance > MaxRange)
	{
		return false;
	}
	FVector Forward = GetOwner()->GetActorForwardVector();
	FVector DirToTarget = (TargetLocation - MonsterLocation).GetSafeNormal();
    
	float DotProduct = FVector::DotProduct(Forward, DirToTarget);
	float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
    
	if (Angle > fovAngle * 0.5f)
	{
		return false;
	}
	TArray<FVector> CheckPoints;
	CheckPoints.Add(TargetLocation + FVector(0,0,10.f));
	CheckPoints.Add(TargetLocation + FVector(0,0,45.f));
	CheckPoints.Add(TargetLocation + FVector(0,0,75.f));
    
    
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Owner);
	for (const FVector& TargetPoint: CheckPoints)
	{
		FHitResult HitResult;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TargetPoint, ECC_Visibility, CollisionParams))
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor)
			{
				if (HitActor == Target || HitActor->ActorHasTag(FProjectTags::Player))
				{
					DrawDebugLine(GetWorld(), EyeLocation, TargetPoint, FColor::Green, false, 0.1f);
					return true;
				}
			}
		}  
	}
	return false;
}

//소리 이벤트 발생시 해당 소리가 감지 범위이내에 있는지 확인
// 소리 이벤트 출력은 플레이어가해줘야 할듯?
void UMonsterSensingComponent::ReportSound(FVector SoundLocation, float VolumeMultiplier)
{
	ABaseMonster* Owner = Cast<ABaseMonster>(GetOwner());
	UMonsterStatusComponent* Status = Owner->FindComponentByClass<UMonsterStatusComponent>();
	
	if (!Status)
	{
		return;
	}
	float FinalHearingRange = Status->GetBaseHearingRange() * VolumeMultiplier;
	float Distance = FVector::Dist(GetOwner()->GetActorLocation(), SoundLocation);
	
	if (Distance <= FinalHearingRange)
	{
		bHeardSound = true;
		LastHeardLocation = SoundLocation;
		
		UE_LOG(LogTemp, Warning, TEXT("[%s] Heard Sound at %s (Volume: %.1f)"), 
			*GetOwner()->GetName(), *SoundLocation.ToString(), VolumeMultiplier);
	}
}


AActor* UMonsterSensingComponent::FindNearestPlayer()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FProjectTags::Player, FoundActors);	
	AActor* NearestPlayer = nullptr;
	float MinDistance = TNumericLimits<float>::Max();
	FVector MyLocation = GetOwner()->GetActorLocation();
	
	for (AActor* Actor: FoundActors)
	{
		if (Actor)
		{
			if (CanSeeTarget(Actor))
			{
				float Distance = FVector::Dist(MyLocation, Actor->GetActorLocation());
				if (Distance < MinDistance)
				{
					MinDistance = Distance;
					NearestPlayer = Actor;
				}
			}
			
		}
	}
	return NearestPlayer;
}
bool UMonsterSensingComponent::IsPlayersGrouping(float Radius, int32 MinCount)
{
	
	return false;
}
bool UMonsterSensingComponent::IsTargetInLight(AActor* Target)
{
	return false;
}