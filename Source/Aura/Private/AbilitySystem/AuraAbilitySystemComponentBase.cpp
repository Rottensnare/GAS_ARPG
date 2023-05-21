// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponentBase.h"

#include "AuraGameplayTags.h"

void UAuraAbilitySystemComponentBase::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                    const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer GPTagContainer;
	EffectSpec.GetAllAssetTags(GPTagContainer);
	

	EffectAssetTags.Broadcast(GPTagContainer);
}

void UAuraAbilitySystemComponentBase::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::EffectApplied);

	//const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	//UE_LOG(LogTemp, Warning, TEXT("Tag: %s"), *GameplayTags.Attribute_Secondary_Armor.ToString())
}
