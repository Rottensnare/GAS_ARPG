// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/CombatManager.h"

#include "NiagaraCommon.h"
#include "AI/Controller/AuraAIController.h"
#include "Characters/AuraEnemy.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACombatManager::ACombatManager()
{
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
	
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this, AAuraEnemy::StaticClass(), OutActors);
	for(AActor* Enemy : OutActors)
	{
		Cast<AAuraEnemy>(Enemy)->CombatManagerRegistration(this);
	}
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
	if(EnemyToRegister != nullptr) SpawnedEnemies.AddUnique(EnemyToRegister);
	UE_LOG(LogTemp, Warning, TEXT("[%s] was registered to the combat manager"), *EnemyToRegister->GetName())
}

void ACombatManager::UnRegisterEnemy(AAuraEnemy* EnemyToUnregister)
{
	if(EnemyToUnregister == nullptr) return;
	if(SpawnedEnemies.Contains(EnemyToUnregister)) SpawnedEnemies.Remove(EnemyToUnregister);
	UE_LOG(LogTemp, Warning, TEXT("[%s] was removed from the combat manager"), *EnemyToUnregister->GetName())
}

