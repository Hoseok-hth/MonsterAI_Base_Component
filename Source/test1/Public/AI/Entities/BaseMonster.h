#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseMonster.generated.h"

class UMonsterDataAsset;
UCLASS()
class TEST1_API ABaseMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseMonster(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	const UMonsterDataAsset* MonsterData;

protected:
	virtual void BeginPlay() override;

	// components 스테이터스, state , sensing, combat,
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Component")
	class UMonsterStatusComponent* StatusComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Component")
	class UMonsterFSMComponent* FSMComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Component")
	class UMonsterSensingComponent* SensingComponent;
	

	
private:
	bool bIsExecutionActive = false;
};