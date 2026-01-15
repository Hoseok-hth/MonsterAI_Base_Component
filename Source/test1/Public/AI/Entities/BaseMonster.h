#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseMonster.generated.h"

class UMonsterFSMComponent;
class UMonsterStatusComponent;
class UMonsterSensingComponent;
class UMonsterDataAsset;
class UAudioComponent;
UCLASS()
class TEST1_API ABaseMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseMonster(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Data")
	const UMonsterDataAsset* MonsterData;
	UMonsterFSMComponent* GetFSMComp()const{ return FSMComponent; };
	UAudioComponent* GetAudioLoopComponent() const { return AudioLoopComponent; }
protected:
	virtual void BeginPlay() override;

	// components 스테이터스, state , sensing, combat,
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Component")
	UMonsterStatusComponent* StatusComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Component")
	UMonsterFSMComponent* FSMComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Component")
	UMonsterSensingComponent* SensingComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Component")
	UAudioComponent* AudioLoopComponent;
	

	
private:
	bool bIsExecutionActive = false;
};