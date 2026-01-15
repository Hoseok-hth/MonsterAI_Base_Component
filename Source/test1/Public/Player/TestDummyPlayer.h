#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/ExecutionTargetInterface.h" // 인터페이스 헤더 포함 필수
#include "TestDummyPlayer.generated.h"


UCLASS()
class TEST1_API ATestDummyPlayer : public ACharacter, public IExecutionTargetInterface
{
	GENERATED_BODY()

public:
	ATestDummyPlayer();

protected:
	virtual void BeginPlay() override;

public:	
	
	virtual void OnCaughtByMonster(class AActor* Monster) override;
	virtual bool IsHidden() const override;
	virtual bool IsDead() const override { return bIsDead; }
private:
	FTimerHandle NoiseTimerHandle;

	void EmitFootstepNoise();

	// 소리 발생 주기 (예: 3초마다)
	float NoiseInterval = 3.0f;

	// 테스트용 소음 크기 (1.0 = 기본)
	float NoiseLoudness = 1.0f;
	bool bIsDead = false;
};