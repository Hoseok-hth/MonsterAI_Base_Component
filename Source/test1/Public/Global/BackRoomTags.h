#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

struct FBackRoomTags

{
public:
	static const FBackRoomTags& Get() { return Instance; }
    
	// 엔진 시작 시 태그를 등록하는 함수
	static void InitializeNativeTags();

	// =============================================================
	// 1. [구형] Actor Tags (FName) - 물리적 식별, 센싱용
	// =============================================================
	static const FName PlayerName;   // "Player"
	static const FName MonsterName;  // "Monster"
	static const FName WallName;     // "Wall"

	// =============================================================
	// 2. [신형] Gameplay Tags - 로직, 상태, 속성용
	// =============================================================
	// 몬스터 상태
	FGameplayTag Status_Stunned;     // Status.Stunned
	FGameplayTag Status_Dead;        // Status.Dead

	// 몬스터 약점
	FGameplayTag Weakness_Light;     // Weakness.Light

	

private:
	static FBackRoomTags Instance;
};