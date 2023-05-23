// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraWidgetController.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if(APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if(AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PlayerState = PC->GetPlayerState<AAuraPlayerState>();
			check(PlayerState)
			UAbilitySystemComponent* AbilitySystemComponent =  PlayerState->GetAbilitySystemComponent();
			UAttributeSet* AttributeSet = PlayerState->GetAttributeSet();
			const FWidgetControllerParams ControllerParams(PC, PlayerState, AbilitySystemComponent, AttributeSet);
			
			return AuraHUD->GetOverlayWidgetController(ControllerParams);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Unable to get UOverlayWidgetController."))
	
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if(APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if(AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PlayerState = PC->GetPlayerState<AAuraPlayerState>();
			check(PlayerState)
			UAbilitySystemComponent* AbilitySystemComponent =  PlayerState->GetAbilitySystemComponent();
			UAttributeSet* AttributeSet = PlayerState->GetAttributeSet();
			const FWidgetControllerParams ControllerParams(PC, PlayerState, AbilitySystemComponent, AttributeSet);
			
			return AuraHUD->GetAttributeMenuWidgetController(ControllerParams);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Unable to get UAttributeMenuWidgetController."))
	
	return nullptr;
}
