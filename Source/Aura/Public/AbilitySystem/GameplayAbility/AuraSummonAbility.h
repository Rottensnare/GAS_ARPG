// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

class AAuraCharacterBase;
/**
 * 
 */
UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Defaults")
	TArray<TSubclassOf<AAuraCharacterBase>> MinionClasses;

	/**	Calculates spawn locations for the minions and returns an array of vectors */
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();

	/**	Max number of minions spawned per cast */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Defaults")
	int32 NumOfMinions = 1;

	/**	Minimum distance where a minion can be spawned */
	float MinSpawnDistance = 80.f;
	
	/**	Maximum distance where a minion can be spawned */
	float MaxSpawnDistance = 200.f;
	
	/**	Angle in which minions are spawned */
	float SpawnSpread = 120.f;
};
