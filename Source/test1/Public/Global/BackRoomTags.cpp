#include "BackRoomTags.h"

FBackRoomTags FBackRoomTags::Instance;

// 1. FName 값 정의 
const FName FBackRoomTags::PlayerName = FName("Player");
const FName FBackRoomTags::MonsterName = FName("Monster");
const FName FBackRoomTags::WallName = FName("Wall");

void FBackRoomTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
    
	Instance.Status_Stunned = Manager.AddNativeGameplayTag(
		TEXT("Status.Stunned"), 
		TEXT("몬스터가 기절한 상태")
	);

	Instance.Status_Dead = Manager.AddNativeGameplayTag(
		TEXT("Status.Dead"), 
		TEXT("사망 상태")
	);

	Instance.Weakness_Light = Manager.AddNativeGameplayTag(
		TEXT("Weakness.Light"), 
		TEXT("손전등 등 빛 속성에 약함")
	);
}