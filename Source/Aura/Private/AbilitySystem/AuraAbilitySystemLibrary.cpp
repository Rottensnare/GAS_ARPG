// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Game/AuraGameModeBase.h"
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

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const ECharacterClass CharacterClass, float Level, const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(AuraGameMode == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AuraGameMode is not valid."))
		return;
	}

	if(AuraGameMode->CharacterClassInfo == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AuraGameMode->CharacterClassInfo is not set."))
		return;
	}
	
	const FCharacterClassDefaultInfo ClassDefaultInfo = AuraGameMode->CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
	GEContextHandle.AddSourceObject(ASC->GetAvatarActor());
	
	const FGameplayEffectSpecHandle PrimaryGESpec = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, GEContextHandle);
	const FGameplayEffectSpecHandle SecondaryGESpec = ASC->MakeOutgoingSpec(AuraGameMode->CharacterClassInfo->SecondaryAttributes, Level, GEContextHandle);
	const FGameplayEffectSpecHandle VitalGESpec = ASC->MakeOutgoingSpec(AuraGameMode->CharacterClassInfo->VitalAttributes, Level, GEContextHandle);
	
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryGESpec.Data.Get());
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryGESpec.Data.Get());
	ASC->ApplyGameplayEffectSpecToSelf(*VitalGESpec.Data.Get());
}
