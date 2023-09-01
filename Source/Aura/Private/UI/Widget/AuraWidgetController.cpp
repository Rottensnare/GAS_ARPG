// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AuraWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponentBase.h"
#include "AbilitySystem/AuraAttributeSetBase.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams)
{
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	AbilitySystemComponent = WidgetControllerParams.AbilitySystemComponent;
	AttributeSet = WidgetControllerParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValues()
{
	
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
	
}

void UAuraWidgetController::BroadcastAbilityInfo()
{
	UAuraAbilitySystemComponentBase* AuraASC = GetAuraASC();
	if(!AuraASC->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this, AuraASC](const FGameplayAbilitySpec& AbilitySpec)
	{
		FAuraAbilityInfo AbilityInfo = AbilityInfoData->FindAbilityInfoForTag(AuraASC->GetAbilityTagFromSpec(AbilitySpec));
		AbilityInfo.InputTag = AuraASC->GetInputTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(AbilityInfo);
	});
	AuraASC->ForEachAbility(BroadcastDelegate);
}

AAuraPlayerController* UAuraWidgetController::GetAuraPC()
{
	if(AuraPlayerController == nullptr)
	{
		AuraPlayerController = CastChecked<AAuraPlayerController>(PlayerController);
	}

	return AuraPlayerController;
}

AAuraPlayerState* UAuraWidgetController::GetAuraPS()
{
	if(AuraPlayerState == nullptr)
	{
		AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	}

	return AuraPlayerState;
}

UAuraAbilitySystemComponentBase* UAuraWidgetController::GetAuraASC()
{
	if(AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = CastChecked<UAuraAbilitySystemComponentBase>(AbilitySystemComponent);
	}

	return AuraAbilitySystemComponent;
}

UAuraAttributeSetBase* UAuraWidgetController::GetAuraAttributeSet()
{
	if(AuraAttributeSet == nullptr)
	{
		AuraAttributeSet = CastChecked<UAuraAttributeSetBase>(AttributeSet);
	}

	return AuraAttributeSet;
}
