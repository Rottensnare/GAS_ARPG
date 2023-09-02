// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponentBase.generated.h"


class UAuraAbilitySystemComponentBase;

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags */)
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponentBase : public UAbilitySystemComponent
{
	GENERATED_BODY()


public:

	void AbilityActorInfoSet();

	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& DefaultAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& DefaultPassiveAbilities);
	bool bStartupAbilitiesGiven = false;

	void ForEachAbility(const FForEachAbility& Delegate);
	
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);
	
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	//Tries to activate a random gameplay ability
	bool TryActivateAbilityByTag(const FGameplayTagContainer& GameplayTagContainer, const bool bAllowRemoteActivation);

	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	
protected:

	virtual void OnRep_ActivateAbilities() override;

	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

	

private:
};
