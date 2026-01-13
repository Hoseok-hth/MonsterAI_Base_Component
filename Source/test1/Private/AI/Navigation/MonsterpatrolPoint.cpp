// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Navigation/MonsterpatrolPoint.h"


// Sets default values
AMonsterpatrolPoint::AMonsterpatrolPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMonsterpatrolPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMonsterpatrolPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

