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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	const UMonsterDataAsset* MonsterData;
	
	UMonsterFSMComponent* GetFSMComp()const{ return FSMComponent; };
	UAudioComponent* GetAudioLoopComponent() const { return AudioLoopComponent; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Event")
	void OnSpecialAbilityStart(); 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Condition")
	void FinishSpecialAbility();
	virtual void FinishSpecialAbility_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Condition")
	bool CanActivateSpecial(); 
	virtual bool CanActivateSpecial_Implementation();
	
	void OnLightHit();
	
	UFUNCTION(BlueprintCallable, Category = "State", meta=(DisplayName="Set Monster State"))
	void SetState(EMonsterState NewState);
	
	UFUNCTION(BlueprintPure, Category = "State", meta=(DisplayName="Get Monster State"))
	EMonsterState GetState() const;
	
	UFUNCTION(CallInEditor, Category = "Data")
	void RefreshMonster();
	
#if WITH_EDITOR
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	
	// components 스테이터스, state , sensing, combat,
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UMonsterStatusComponent* StatusComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UMonsterFSMComponent* FSMComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UMonsterSensingComponent* SensingComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UAudioComponent* AudioLoopComponent;
	

	
private:
	bool bIsExecutionActive = false;
};