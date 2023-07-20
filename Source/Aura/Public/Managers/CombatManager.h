// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CombatManager.generated.h"



class AAuraEnemy;
class AAuraAIController;
class AAuraCharacterBase;


USTRUCT(BlueprintType)
struct FSquad
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SquadID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AAuraEnemy* SquadLeader = nullptr;

	TArray<TWeakObjectPtr<AAuraEnemy>> Minions;

	
};

//NOTE: You might wonder why this is a pawn when it will not have a body? Simple answer, Behavior tree, complicated answer, I have no idea what I'm doing.
UCLASS()
class AURA_API ACombatManager : public APawn
{
	GENERATED_BODY()

public:

	ACombatManager();

protected:

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

public:	

	virtual void Tick(float DeltaTime) override;


	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	TArray<TWeakObjectPtr<AAuraEnemy>> SpawnedEnemies;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Squads")
	TArray<FSquad> Squads;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom AI")
	TObjectPtr<AAuraAIController> AIController;
	
	UFUNCTION(BlueprintCallable)
	void RegisterEnemy(AAuraEnemy* EnemyToRegister);
	UFUNCTION(BlueprintCallable)
	void UnRegisterEnemy(AAuraEnemy* EnemyToUnregister);
	
};
