// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound)
{
	for(const auto& InputAction : AbilityInputActions)
	{
		if(InputAction.InputTag.MatchesTagExact(InputTag))
		{
			return InputAction.InputAction;
		}
	}

	if(bLogNotFound) UE_LOG(LogTemp, Error, TEXT("UAuraInputConfig::FindAbilityInputActionForTag:	InputAction not found with Tag: %s"), *InputTag.ToString())
	return nullptr;
}
