// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Animation/AnimationNotify/AnimNotifyMonsterStep.h"
#include "AI/Data/MonsterDataAsset.h"
#include "AI/Entities/BaseMonster.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotifyMonsterStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	ABaseMonster* Monster = Cast<ABaseMonster>(MeshComp->GetOwner());
	if (Monster && Monster->MonsterData)
	{
		
		USoundBase* SoundToPlay = bIsRunning ? Monster->MonsterData->RunSound : Monster->MonsterData->WalkSound;

		
		if (SoundToPlay)
		{
			UGameplayStatics::PlaySoundAtLocation(MeshComp->GetWorld(), SoundToPlay, Monster->GetActorLocation());
		}
	}
}
