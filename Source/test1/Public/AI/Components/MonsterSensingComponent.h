#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MonsterSensingComponent.generated.h"

class UMonsterStatusComponent;
class UMonsterDataAsset;
class ABaseMonster;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEST1_API UMonsterSensingComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI | Component")
	UMonsterStatusComponent* Status;
	
	
public:
	UMonsterSensingComponent();
	UPROPERTY()
	const UMonsterDataAsset* MonsterData;
	
	//sight 시야 기반 탐색
	bool CanSeeTarget(AActor* Target);
	
	//hearing 청각 기반 탐색
	void ReportSound(FVector SoundLocation, float VolumeMultiplier);
	bool WasSoundHeard() const {return bHeardSound;}
	FVector GetLastSoundLocation() const{ return LastHeardLocation; }
	void ResetSoundFlag(){ bHeardSound = false;};
	
	//event 트리거 기반 탐색
	void SetTriggerAlert(bool bAlert){ bTriggered = bAlert;}
	bool GetIsTriggerd() const {return bTriggered;}
	
	// 가장 가까운 보이는 플레이어 반환
	AActor* FindNearestPlayer();
	
	
	//기획에서 뭉쳐있으면 쫒아가는 부분 구현x
	bool IsPlayersGrouping(float Radius, int32 MinCount);
	bool IsTargetInLight(AActor* Target);
	
	private:
	//sound
	bool bHeardSound = false;
	FVector LastHeardLocation;
	
	//event
	bool bTriggered = false;
	
private:
	UPROPERTY()
	ABaseMonster* Owner;
	
	
	
};