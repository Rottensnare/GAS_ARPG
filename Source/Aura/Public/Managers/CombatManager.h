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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SquadID = -1;

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

	FSquad()
	{
		SquadID = -1;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SquadID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FCombatant SquadLeader;

	TArray<FCombatant> Members;

	bool IsPartOfSquad(const FCombatant& InCombatant)
	{
		for(const FCombatant& Combatant : Members)
		{
			if(InCombatant == Combatant) return true;
		}
		return false;
	}

	bool operator==(const FSquad InSquad) const
	{
		if(InSquad.SquadID == SquadID) return true;
		return false;
	}
};

inline FSquad DefaultSquad;

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

	void InitialDivideIntoSquads(const float Eps = 500.f, const int32 MinPts = 3);
	TArray<FCombatant> FindNeighbours(const FCombatant& InCombatant, const float Eps = 500.f);

	FSquad& FindSquadWithID(const int32 InID);

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

inline FSquad& ACombatManager::FindSquadWithID(const int32 InID)
{
	for(FSquad& Squad : Squads)
	{
		if(Squad.SquadID == InID) return Squad;
	}
	return DefaultSquad;
}

template<typename Type, typename Array>
bool RemoveFromArray(Type& Object, Array& ObjectArray)
{
	const int32 NumRemoved = ObjectArray.RemoveAll([&Object](const Type& InObject)
   {
	   return InObject.Enemy == Object.Enemy;
   });

	return NumRemoved > 0;
}
