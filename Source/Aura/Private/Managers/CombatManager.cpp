// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/CombatManager.h"

#include "NiagaraCommon.h"
#include "AI/Controller/AuraAIController.h"
#include "Aura/AuraLogChannels.h"
#include "Characters/AuraEnemy.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACombatManager::ACombatManager()
{
	// leaving it true for now
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 2.f;
	FCombatant::CurrentID = 0;
}

void ACombatManager::BeginPlay()
{
	Super::BeginPlay();

	
}

void ACombatManager::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if(HasAuthority() == false) return;
	
	AIController = Cast<AAuraAIController>(NewController);
	Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this))->SetCombatManagerReady(true);
}

void ACombatManager::InitialDivideIntoSquads(const TArray<bool>& Visited, const FVector& Cluster, float Eps, int32 MinPoints)
{
	
}

FCombatant ACombatManager::FindCombatant(const AAuraEnemy* InEnemy)
{
	for(FCombatant Combatant : SpawnedCombatants)
	{
		if(Combatant == InEnemy)
		{
			return Combatant;
		}
	}
	

	const FCombatant NullCombatant = FCombatant();
	
	return NullCombatant;
}

bool ACombatManager::RemoveCombatant(const FCombatant& InCombatant)
{
	const int32 NumRemoved = SpawnedCombatants.RemoveAll([&InCombatant](const FCombatant& Combatant)
   {
	   return Combatant.Enemy == InCombatant.Enemy;
   });

	return NumRemoved > 0;
}

// Called every frame
void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
}

void ACombatManager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
}

void ACombatManager::RegisterEnemy(AAuraEnemy* EnemyToRegister)
{
	if(!IsValid(EnemyToRegister)) return;
	if(EnemyToRegister != nullptr) SpawnedCombatants.Emplace(FCombatant(EnemyToRegister));
	UE_LOG(LogAura, Warning, TEXT("[%s] was registered to the combat manager"), *EnemyToRegister->GetName())
}

void ACombatManager::UnRegisterEnemy(AAuraEnemy* EnemyToUnregister)
{
	if(!IsValid(EnemyToUnregister)) return;
		const bool bSuccess = RemoveCombatant(FCombatant(EnemyToUnregister));
		if(bSuccess) UE_LOG(LogAura, Warning, TEXT("[%s] was removed from the combat manager"), *EnemyToUnregister->GetName())
		else UE_LOG(LogAura, Error, TEXT("Failed to remove [%s] from the combat manager"), *EnemyToUnregister->GetName())
}

