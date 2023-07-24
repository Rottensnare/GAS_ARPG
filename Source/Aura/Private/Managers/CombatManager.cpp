// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/CombatManager.h"

#include "NiagaraCommon.h"
#include "AI/Controller/AuraAIController.h"
#include "Characters/AuraEnemy.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACombatManager::ACombatManager()
{
	// leaving it true for now
	PrimaryActorTick.bCanEverTick = true;
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
	if(EnemyToRegister != nullptr) SpawnedEnemies.Emplace(EnemyToRegister);
	UE_LOG(LogTemp, Warning, TEXT("[%s] was registered to the combat manager"), *EnemyToRegister->GetName())
}

void ACombatManager::UnRegisterEnemy(AAuraEnemy* EnemyToUnregister)
{
	if(!IsValid(EnemyToUnregister)) return;
	if(SpawnedEnemies.Contains(EnemyToUnregister)) SpawnedEnemies.Remove(EnemyToUnregister);
	if(EnemyToUnregister) UE_LOG(LogTemp, Warning, TEXT("[%s] was removed from the combat manager"), *EnemyToUnregister->GetName())
}

