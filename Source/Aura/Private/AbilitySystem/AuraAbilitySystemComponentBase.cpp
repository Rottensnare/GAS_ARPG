// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponentBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/GameplayAbility/AuraGameplayAbility.h"
#include "Aura/AuraLogChannels.h"
#include "Debug/DebugFunctionLibrary.h"
#include "Interfaces/PlayerInterface.h"

void UAuraAbilitySystemComponentBase::AddCharacterAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& DefaultAbilities)
{
	for(const auto AbilityClass : DefaultAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if(const UAuraGameplayAbility* AuraGameplayAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraGameplayAbility->StartupInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true; 
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponentBase::AddCharacterPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& DefaultPassiveAbilities)
{
	for(const auto AbilityClass : DefaultPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAuraAbilitySystemComponentBase::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for(const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

void UAuraAbilitySystemComponentBase::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return;

	for(auto& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if(AbilitySpec.IsActive() == false)
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponentBase::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return;

	for(auto& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponentBase::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if(!AbilitySpec.Ability) return FGameplayTag();
	for(const auto& Tag : AbilitySpec.Ability.Get()->AbilityTags)
	{
		if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
		{
			return Tag;
		}
	}

	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponentBase::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for(const auto& Tag : AbilitySpec.DynamicAbilityTags)
	{
		if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}

	return FGameplayTag();
}

void UAuraAbilitySystemComponentBase::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if(GetAvatarActor()->Implements<UPlayerInterface>() && IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
	{
		ServerUpgradeAttribute(AttributeTag);
	}
}

void UAuraAbilitySystemComponentBase::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
	if(GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

bool UAuraAbilitySystemComponentBase::TryActivateAbilityByTag(const FGameplayTagContainer& GameplayTagContainer,
	const bool bAllowRemoteActivation)
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, AbilitiesToActivate);
	UDebugFunctionLibrary::ShuffleArray(AbilitiesToActivate);

	//Try activating abilities in order from the shuffled array of ability specs
	for (const FGameplayAbilitySpec* GameplayAbilitySpec : AbilitiesToActivate)
	{
		if(TryActivateAbility(GameplayAbilitySpec->Handle, bAllowRemoteActivation))
		{
			return true;
		}
	}

	return false;
}

FGameplayTag UAuraAbilitySystemComponentBase::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for(FGameplayTag StatusTag : AbilitySpec.DynamicAbilityTags)
	{
		if(StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return StatusTag;
		}
	}

	return FGameplayTag::EmptyTag;
}

void UAuraAbilitySystemComponentBase::UpdateAbilityStatuses(const int32 Level)
{
	
	for(const FAuraAbilityInfo& AbilityInfo : UAuraAbilitySystemLibrary::GetAbilityInfo(this)->AbilityInformation)
	{
		if(AbilityInfo.LevelRequirement > Level) continue;
		if(AbilityInfo.AbilityTag.IsValid() == false) continue;
		
		if(GetSpecFromAbilityTag(AbilityInfo.AbilityTag) == nullptr)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityInfo.GameplayAbilityClass, 1);
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec);
			ClientUpdateAbilityStatus(AbilityInfo.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible);
		}
	}
}

FGameplayAbilitySpec* UAuraAbilitySystemComponentBase::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		//TODO: Instead of this, just do this work at compile time.
		//NOTE: Only doing it this way since the instructor is doing it this way. This seems to waste performance
		
		for(FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if(Tag.MatchesTag(AbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}

	return nullptr;
}

void UAuraAbilitySystemComponentBase::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if(!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
	}
	
}

void UAuraAbilitySystemComponentBase::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag)
{
	AbilityStatusChangedDelegate.Broadcast(AbilityTag, StatusTag);
}

void UAuraAbilitySystemComponentBase::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                         const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer GPTagContainer;
	EffectSpec.GetAllAssetTags(GPTagContainer);
	
	EffectAssetTags.Broadcast(GPTagContainer);
}

void UAuraAbilitySystemComponentBase::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::ClientEffectApplied);

	//const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	//UE_LOG(LogTemp, Warning, TEXT("Tag: %s"), *GameplayTags.Attribute_Secondary_Armor.ToString())
}
