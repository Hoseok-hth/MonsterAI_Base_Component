#include "AI/Components/MonsterSensingComponent.h"
#include "AI/Entities/BaseMonster.h"
#include "AI/Components/MonsterStatusComponent.h"
#include "GameFramework/Character.h"
#include "Global/Define.h"
#include "AI/Data/MonsterDataAsset.h"


void UMonsterSensingComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = Cast<ABaseMonster>(GetOwner());
	Status = Owner->FindComponentByClass<UMonsterStatusComponent>();
	MonsterData = Owner->MonsterData;
}

UMonsterSensingComponent::UMonsterSensingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}


bool UMonsterSensingComponent::CanSeeTarget(AActor* Target)
{
	if (!Target ||!Status)
	{
		return false;
	}
	
	float MaxRange = MonsterData->BaseDetectionRange;
	float fovAngle = MonsterData->ViewAngle;
    
	FVector MonsterLocation = GetOwner()->GetActorLocation();
	FVector EyeLocation = MonsterLocation + FVector(0,0,MonsterData->EyeHeight);
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


void UMonsterSensingComponent::ReportSound(FVector SoundLocation, float VolumeMultiplier)
{
	
	if (!Owner || !Status)
	{
		return;
	}
	float FinalHearingRange = MonsterData->BaseHearingRange * VolumeMultiplier;
	float Distance = FVector::Dist(Owner->GetActorLocation(), SoundLocation);
	
	if (Distance <= FinalHearingRange)
	{
		bHeardSound = true;
		LastHeardLocation = SoundLocation;
		
		UE_LOG(LogTemp, Warning, TEXT("[%s] Heard Sound at %s (Volume: %.1f)"), 
			*Owner->GetName(), *SoundLocation.ToString(), VolumeMultiplier);
	}
}


AActor* UMonsterSensingComponent::FindNearestPlayer()
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	AActor* NearestPlayer = nullptr;
	
	float MinDistanceSq = TNumericLimits<float>::Max();
	FVector MyLocation = GetOwner()->GetActorLocation();

	
	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Iterator->Get();
		
		if (PC && PC->GetPawn())
		{
			AActor* TargetPawn = PC->GetPawn();

			
			if (!TargetPawn->ActorHasTag(FProjectTags::Player))
			{
				continue;
			}

			
			float DistSq = FVector::DistSquared(MyLocation, TargetPawn->GetActorLocation());

			
			if (DistSq >= MinDistanceSq)
			{
				continue;
			}

			
			if (CanSeeTarget(TargetPawn))
			{
				MinDistanceSq = DistSq;
				NearestPlayer = TargetPawn;
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