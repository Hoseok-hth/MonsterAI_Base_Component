#include "Player/TestDummyPlayer.h"

#include "AI/Components/MonsterSensingComponent.h"
#include "AI/Entities/BaseMonster.h"
#include "Global/BackRoomTags.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ATestDummyPlayer::ATestDummyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add(FBackRoomTags::PlayerName);

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void ATestDummyPlayer::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(NoiseTimerHandle, this, &ATestDummyPlayer::EmitFootstepNoise, NoiseInterval, true);
}

void ATestDummyPlayer::OnCaughtByMonster(AActor* Monster)
{
	if (bIsDead) return;
	bIsDead = true;

	UE_LOG(LogTemp, Error, TEXT("!!!! 몬스터에게 잡혔습니다! 처형 당함 !!!!"));

	
	if (GetMesh())
	{
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetSimulatePhysics(true);
	}

	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().ClearTimer(NoiseTimerHandle);
    
	
	DetachFromControllerPendingDestroy();
}

bool ATestDummyPlayer::IsHidden() const
{
	return false;
}

void ATestDummyPlayer::EmitFootstepNoise()
{
	// [1] 시각적 디버깅: 소리가 났음을 눈으로 확인 (파란색 구체)
	FVector MyLocation = GetActorLocation();
	DrawDebugSphere(GetWorld(), MyLocation, 50.0f, 12, FColor::Blue, false, 1.0f);
	UE_LOG(LogTemp, Warning, TEXT("[TestDummy] 쿵! 소음 발생"));

	// [2] 주변의 모든 몬스터를 찾아서 소리를 전달
	// (최적화를 위해 OverlapSphere를 써야 하지만, 테스트용이니 GetAllActors로 단순화)
	TArray<AActor*> FoundMonsters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseMonster::StaticClass(), FoundMonsters);

	for (AActor* Actor : FoundMonsters)
	{
		ABaseMonster* Monster = Cast<ABaseMonster>(Actor);
		if (Monster)
		{
			// 몬스터의 감각 컴포넌트 가져오기
			// (BaseMonster에 GetSensingComponent()가 없다면 FindComponentByClass 사용)
			UMonsterSensingComponent* Sensing = Monster->FindComponentByClass<UMonsterSensingComponent>();
            
			if (Sensing)
			{
				// [핵심] 몬스터의 귀에 직접 소리 정보를 꽂아줍니다.
				// 몬스터 코드는 이 함수를 통해 bHeardSound = true가 되고
				// FSM의 HandleIdle -> EarChase 로직이 작동하게 됩니다.
				Sensing->ReportSound(MyLocation, NoiseLoudness);
			}
		}
	}
}
