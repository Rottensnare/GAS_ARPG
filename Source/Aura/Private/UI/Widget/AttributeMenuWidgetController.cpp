// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponentBase.h"
#include "AbilitySystem/AuraAttributeSetBase.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSetBase* AS = CastChecked<UAuraAttributeSetBase>(AttributeSet);

	for(auto& Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda([this, Pair](const FOnAttributeChangeData& Data)
		{
			BroadcastAttributeInfo(Pair.Key, Pair.Value());
		});
	}
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->OnAttributePointsChangedDelegate.AddLambda([this](const int32 Points)
	{
		AttributePointsChangedDelegate.Broadcast(Points);
	});
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSetBase* AS = CastChecked<UAuraAttributeSetBase>(AttributeSet);

	check(AttributeInfo)
	
	for(auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	
	const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AttributePointsChangedDelegate.Broadcast(AuraPlayerState->GetAttributePoints());
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UAuraAbilitySystemComponentBase* const AuraASC = CastChecked<UAuraAbilitySystemComponentBase>(AbilitySystemComponent);
	AuraASC->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& InTag, const FGameplayAttribute& GameplayAttribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(InTag);
	Info.AttributeValue = GameplayAttribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
