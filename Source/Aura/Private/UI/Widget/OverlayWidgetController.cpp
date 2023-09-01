// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponentBase.h"
#include "AbilitySystem/AuraAttributeSetBase.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSetBase* AuraAS = GetAuraAttributeSet();
	OnHealthChanged.Broadcast(AuraAS->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAS->GetMana());
	OnMaxManaChanged.Broadcast(AuraAS->GetMaxMana());
	
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	
	const UAuraAttributeSetBase* AuraAS = GetAuraAttributeSet();

	//	Lambdas
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	AuraAS->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
		
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	AuraAS->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	AuraAS->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAS->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			}
		);

	UAuraAbilitySystemComponentBase* AuraASC = GetAuraASC();
	if(AuraASC)
	{
		AuraASC->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			//check(GEngine)
			for(const auto& Tag : AssetTags)
			{
				// If has a message tag
				if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Message"))))
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString()));
					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		}
	);
		if(AuraASC->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		else
		{
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastInitialValues);
		}
	}

	AAuraPlayerState* AuraPS = GetAuraPS();
	AuraPS->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);

	AuraPS->OnLevelChangedDelegate.AddLambda([this](const int32 InLevel)
	{
		OnPlayerLevelChangedDelegate.Broadcast(InLevel);
	});
	
}

void UOverlayWidgetController::OnXPChanged(const int32 NewXP)
{
	const AAuraPlayerState* AuraPS = GetAuraPS();
	const ULevelUpInfo* LevelUpInfo = AuraPS->LevelUpInfo;
	const int32 NewLevel = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

	if(NewLevel <= MaxLevel && NewLevel > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[NewLevel].LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[NewLevel-1].LevelUpRequirement;

		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;
		const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);
		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}
}
