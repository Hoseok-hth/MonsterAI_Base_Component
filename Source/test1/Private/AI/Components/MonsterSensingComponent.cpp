#include "AI/Components/MonsterSensingComponent.h"
#include "AI/Entities/BaseMonster.h"
#include "AI/Components/MonsterStatusComponent.h"
#include "GameFramework/Character.h"
#include "Global/BackRoomTags.h"
#include "AI/Data/MonsterDataAsset.h"
#include "Interfaces/ExecutionTargetInterface.h"
#include "Kismet/GameplayStatics.h"


void UMonsterSensingComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = Cast<ABaseMonster>(GetOwner());
	if (Owner)
	{
		Status = Owner->FindComponentByClass<UMonsterStatusComponent>();
		MonsterData = Owner->MonsterData;
	}
	
}

UMonsterSensingComponent::UMonsterSensingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}


bool UMonsterSensingComponent::CanSeeTarget(AActor* Target)
{
	if (!Target ||!Status)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Target or Status Comp"));
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
		UE_LOG(LogTemp, Warning, TEXT("Actor Distance Too Far"));

		return false;
	}
	FVector Forward = GetOwner()->GetActorForwardVector();
	FVector DirToTarget = (TargetLocation - MonsterLocation).GetSafeNormal();
    
	float DotProduct = FVector::DotProduct(Forward, DirToTarget);
	float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
    
	if (Angle > fovAngle * 0.5f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Out of Angle"));
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
				if (HitActor == Target || HitActor->ActorHasTag(FBackRoomTags::PlayerName))
				{
					DrawDebugLine(GetWorld(), EyeLocation, TargetPoint, FColor::Green, false, 0.1f);
					return true;
				}else
				{
					DrawDebugLine(GetWorld(), EyeLocation, HitResult.ImpactPoint, FColor::Red, false, 0.1f);
					UE_LOG(LogTemp, Warning, TEXT("Blocked by: %s"), *HitActor->GetName());
				}
			}else
			{
				DrawDebugLine(GetWorld(), EyeLocation, TargetPoint, FColor::Red, false, 0.1f);
				UE_LOG(LogTemp, Warning, TEXT("Hit Nothing (Passed through)"));
			}
		}  
	}
	UE_LOG(LogTemp, Warning, TEXT("Actor behind something"));
	return false;
}


void UMonsterSensingComponent::ReportSound(FVector SoundLocation, float VolumeMultiplier)
{
	
	if (!Owner || !Status)
	{
		return;
	}
	float FinalHearingRange = Status->GetBaseHearingRange() * VolumeMultiplier;
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

	TArray<AActor*> Candidates;
	
	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Iterator->Get();
		
		if (PC && PC->GetPawn())
		{
			if (AActor* TargetPawn = PC->GetPawn(); TargetPawn->ActorHasTag(FBackRoomTags::PlayerName))
			{
				Candidates.Add(TargetPawn);
			}
			
		}
	}
#if WITH_EDITOR
	TArray<AActor*> DummyActors;
	UGameplayStatics::GetAllActorsWithTag(World, FBackRoomTags::PlayerName, DummyActors);
    
	for (AActor* Dummy : DummyActors)
	{
		Candidates.AddUnique(Dummy);
	}
	
	
#endif
	
	for (AActor* Candidate : Candidates)
	{
		IExecutionTargetInterface* TargetInterface = Cast<IExecutionTargetInterface>(Candidate);
		if (!TargetInterface) continue;
		if (TargetInterface->IsDead()) 
		{
			continue;
		}
		float DistSq = FVector::DistSquared(MyLocation, Candidate->GetActorLocation());
		if (DistSq >= MinDistanceSq)
		{
			continue;
		}

			
		if (CanSeeTarget(Candidate))
		{
			MinDistanceSq = DistSq;
			NearestPlayer = Candidate;
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