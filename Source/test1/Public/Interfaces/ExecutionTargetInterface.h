#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ExecutionTargetInterface.generated.h"

UINTERFACE(MinimalAPI)
class UExecutionTargetInterface : public UInterface { GENERATED_BODY() };

class TEST1_API IExecutionTargetInterface
{
	GENERATED_BODY()

public:
	virtual void OnCaughtByMonster(AActor* Killer) = 0;
	virtual bool IsHidden() const = 0;
	virtual bool IsDead() const { return false; }
};