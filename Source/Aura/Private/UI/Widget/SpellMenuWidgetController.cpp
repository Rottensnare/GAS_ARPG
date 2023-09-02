// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponentBase.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->AbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
	{
		if(AbilityInfoData.Get())
		{
			FAuraAbilityInfo Info = AbilityInfoData.Get()->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});
}
