#include "AI/Components/MonsterSensingComponent.h"
#include "AI/Entities/BaseMonster.h"
#include "AI/Components/MonsterStatusComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

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
	ABaseMonster* Owner = Cast<ABaseMonster>(GetOwner());
	UMonsterStatusComponent* Status = Owner->FindComponentByClass<UMonsterStatusComponent>();
	if (!Status)
	{
		return false;
	}
	float MaxRange = Status->GetBaseDetectionRange();
	float fovAngle = Status->GetViewAngle();
	
	FVector MonsterLocation = GetOwner()->GetActorLocation();
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
	
	if (Angle < fovAngle * 0.5f)
	{
		return false;
	}
	
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Owner);
	
	if (GetWorld()->LineTraceSingleByChannel(HitResult,MonsterLocation,TargetLocation,ECC_Visibility,CollisionParams))
	{
		if (HitResult.GetActor() != Target)
		{
			return false;
		}
	}
	return true;
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
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), FoundActors);
	
	AActor* NearestPlayer = nullptr;
	float MinDistance = TNumericLimits<float>::Max();
	return nullptr;
}
bool UMonsterSensingComponent::IsPlayersGrouping(float Radius, int32 MinCount) { return false; }
bool UMonsterSensingComponent::IsTargetInLight(AActor* Target) { return false; }