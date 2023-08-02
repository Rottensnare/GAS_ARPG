// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CombatManager.generated.h"



class AAuraEnemy;
class AAuraAIController;
class AAuraCharacterBase;

UENUM(BlueprintType) 
enum class ESquadRole : uint8
{
	DefaultMax,
	
	Grunt,
	Leader,
	Vanguard,
	Support
};

USTRUCT(BlueprintType)
struct FCombatant
{
	GENERATED_BODY()

	inline static long long CurrentID = 0;
	
	FCombatant()
	{
		CombatantID = -1;
		Role = ESquadRole::DefaultMax;
		Enemy = nullptr;
		bInASquad = false;
	};
	
	FCombatant(AAuraEnemy* InEnemy)
	{
		Enemy = InEnemy;
		CurrentID++;
		CombatantID = CurrentID;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CombatantID = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AAuraEnemy* Enemy = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ESquadRole Role = ESquadRole::Grunt;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bInASquad = false;

	static bool IsNullCombatant(const FCombatant& InCombatant)
	{
		return	InCombatant.CombatantID == -1 ||
				InCombatant.Enemy == nullptr;
	}

	bool operator==(const AAuraEnemy* InEnemy) const
	{
		if(InEnemy == Enemy) return true;

		return false;
	}
	
	bool operator==(const FCombatant InCombatant) const
	{
		if(InCombatant.Enemy == Enemy) return true;

		return false;
	}
	
};

USTRUCT(BlueprintType)
struct FSquad
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SquadID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FCombatant SquadLeader;

	TArray<FCombatant> Members;

	
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

	void InitialDivideIntoSquads(const TArray<bool>& Visited, const FVector& Cluster, float Eps, int32 MinPoints);
	TArray<FSquad> FindNeighbors();

	FCombatant FindCombatant(const AAuraEnemy* InEnemy);
	bool RemoveCombatant(const FCombatant& InCombatant);

public:	

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FCombatant> SpawnedCombatants;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Squads")
	TArray<FSquad> Squads;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom AI")
	TObjectPtr<AAuraAIController> AIController;
	
	UFUNCTION(BlueprintCallable)
	void RegisterEnemy(AAuraEnemy* EnemyToRegister);
	UFUNCTION(BlueprintCallable)
	void UnRegisterEnemy(AAuraEnemy* EnemyToUnregister);
	
	
	
};
