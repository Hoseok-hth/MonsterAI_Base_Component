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
	// 몬스터에게 잡혔을 때 호출될 함수 (처형 당하는 자가 구현)
	virtual void OnCaughtByMonster(AActor* Killer) = 0;
};