// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AuraAbilityTypes.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Game/AuraGameModeBase.h"
#include "Interfaces/CombatInterface.h"
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

	UE_LOG(LogTemp, Warning, TEXT("UAuraAbilitySystemLibrary:	Unable to get UOverlayWidgetController."))
	
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

	UE_LOG(LogTemp, Warning, TEXT("UAuraAbilitySystemLibrary:	Unable to get UAttributeMenuWidgetController."))
	
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const ECharacterClass CharacterClass, float Level, const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(AuraGameMode == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UAuraAbilitySystemLibrary:	AuraGameMode is not valid."))
		return;
	}

	if(AuraGameMode->CharacterClassInfo == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UAuraAbilitySystemLibrary:	AuraGameMode->CharacterClassInfo is not set."))
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

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(AuraGameMode == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UAuraAbilitySystemLibrary:	AuraGameMode is not valid."))
		return;
	}

	if(AuraGameMode->CharacterClassInfo == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UAuraAbilitySystemLibrary:	AuraGameMode->CharacterClassInfo is not set."))
		return;
	}

	if(ASC == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UAuraAbilitySystemLibrary:	ASC is not set."))
		return;
	}
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(ASC->GetAvatarActor());
	if(CombatInterface == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UAuraAbilitySystemLibrary:	CombatInterface is not set."))
		return;
	}
	UCharacterClassInfo* CharacterClassInfo = AuraGameMode->CharacterClassInfo;
	for(const auto& Ability : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec GEAbilitySpec = FGameplayAbilitySpec(Ability);
		ASC->GiveAbility(GEAbilitySpec);
	}
	const FCharacterClassDefaultInfo& ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	if(CharacterClassInfo)
	{
		for(auto& AbilityClass : ClassDefaultInfo.ClassAbilities)
		{
			FGameplayAbilitySpec GameplayAbilitySpec = FGameplayAbilitySpec(AbilityClass, CombatInterface->GetCharacterLevel());
			ASC->GiveAbility(GameplayAbilitySpec);
		}
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(AuraGameMode) return AuraGameMode->CharacterClassInfo;

	return nullptr;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(AuraGameplayEffectContext)
	{
		return AuraGameplayEffectContext->IsBlockedHit();
	}

	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(AuraGameplayEffectContext)
	{
		return AuraGameplayEffectContext->IsCriticalHit();
	}

	return false;
}

int32 UAuraAbilitySystemLibrary::GetEffectModifierBits(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(AuraGameplayEffectContext)
	{
		return AuraGameplayEffectContext->GetEffectModifierBits();
	}

	return -1;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle,
	const bool bIsBlockedHit)
{
	FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(AuraGameplayEffectContext)
	{
		AuraGameplayEffectContext->SetIsBlockedHit(bIsBlockedHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
	const bool bIsCriticalHit)
{
	FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(AuraGameplayEffectContext)
	{
		AuraGameplayEffectContext->SetIsCriticalHit(bIsCriticalHit);
	}
}

void UAuraAbilitySystemLibrary::SetEffectModifierBits(FGameplayEffectContextHandle& EffectContextHandle,
	const int32 InEffectModifierBits)
{
	FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if(AuraGameplayEffectContext)
	{
		AuraGameplayEffectContext->SetEffectModifierBits(InEffectModifierBits);
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
	TArray<AActor*>& OutActors, const TArray<AActor*>& ActorsToIgnore, const float Radius, const FVector& SphereOrigin)
{
	const UWorld* CurrentWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActors(ActorsToIgnore);
	FCollisionObjectQueryParams CollisionObjectQueryParams;
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	
	CurrentWorld->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, CollisionObjectQueryParams, FCollisionShape::MakeSphere(Radius), CollisionQueryParams);
	for(const FOverlapResult& OverlapResult : Overlaps)
	{
		check(OverlapResult.GetActor())
		if(OverlapResult.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(OverlapResult.GetActor()))
		{
			OutActors.AddUnique(OverlapResult.GetActor());
		}
	}
}


