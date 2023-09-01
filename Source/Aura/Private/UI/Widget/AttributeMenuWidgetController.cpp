// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponentBase.h"
#include "AbilitySystem/AuraAttributeSetBase.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSetBase* AS = GetAuraAttributeSet();

	for(auto& Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda([this, Pair](const FOnAttributeChangeData& Data)
		{
			BroadcastAttributeInfo(Pair.Key, Pair.Value());
		});
	}
	AAuraPlayerState* AuraPS = GetAuraPS();
	AuraPS->OnAttributePointsChangedDelegate.AddLambda([this](const int32 Points)
	{
		AttributePointsChangedDelegate.Broadcast(Points);
	});
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSetBase* AS = GetAuraAttributeSet();

	check(AttributeInfo)
	
	for(auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	
	const AAuraPlayerState* AuraPS = GetAuraPS();
	AttributePointsChangedDelegate.Broadcast(AuraPS->GetAttributePoints());
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UAuraAbilitySystemComponentBase* const AuraASC = GetAuraASC();
	AuraASC->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& InTag, const FGameplayAttribute& GameplayAttribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(InTag);
	Info.AttributeValue = GameplayAttribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
