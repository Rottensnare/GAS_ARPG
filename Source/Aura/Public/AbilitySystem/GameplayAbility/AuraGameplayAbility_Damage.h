// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/AuraGameplayAbility.h"
#include "AuraGameplayAbility_Damage.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility_Damage : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	bool GetFriendlyFire() const {return bFriendlyFire;}

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

	/**	If true, will only do damage to non-friendly actors
	 *	Actor with "Player" tag will be friendly with other Actors with tag "Player"
	 *	Actor with "Enemy" tag will be friendly with other Actors with tag "Enemy"
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	bool bFriendlyFire = false;
	
};
