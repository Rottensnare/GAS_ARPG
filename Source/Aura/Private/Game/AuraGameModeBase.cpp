// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AuraGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "Managers/CombatManager.h"

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this, ACombatManager::StaticClass(), OutActors);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	if(OutActors.IsEmpty())
	{
		CombatManager = GetWorld()->SpawnActor<ACombatManager>(CombatManagerClass, FVector(0.f, 0.f, -100.f), FRotator::ZeroRotator, SpawnParams);
	}
	else
	{
		CombatManager = Cast<ACombatManager>(OutActors[0]);
	}
}

ACombatManager* AAuraGameModeBase::GetCombatManager()
{
	if(CombatManager) return CombatManager;

	CombatManager = GetWorld()->SpawnActor<ACombatManager>(ACombatManager::StaticClass(), FVector(0.f, 0.f, -100.f), FRotator::ZeroRotator);
	return CombatManager;
}
