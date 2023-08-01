// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AuraAbilityTypes.h"
#include "GameplayDebuggerTypes.h"
#include "ToolContextInterfaces.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Aura/AuraLogChannels.h"
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
		UE_LOG(LogTemp, Error, TEXT("%hs:	AuraGameMode is not valid."), __FUNCTION__)
		return;
	}

	if(AuraGameMode->CharacterClassInfo == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%hs:	AuraGameMode->CharacterClassInfo is not set."), __FUNCTION__)
		return;
	}

	if(ASC == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%hs:	ASC is not set."), __FUNCTION__)
		return;
	}
	if(ASC->GetAvatarActor()->Implements<UCombatInterface>() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("%hs: Avatar actor does not implement ICombatInterface."), __FUNCTION__)
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
			FGameplayAbilitySpec GameplayAbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetCharacterLevel(ASC->GetAvatarActor()));
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

bool UAuraAbilitySystemLibrary::IsFriend(AActor* FirstActor, AActor* SecondActor)
{
	if(FirstActor == nullptr || SecondActor == nullptr) return false;
	const FName EnemyTag("Enemy");
	const FName PlayerTag("Player");
	
	//If both have Enemy tag or both have Player tag, return true, otherwise return false
	if((FirstActor->ActorHasTag(EnemyTag) && SecondActor->ActorHasTag(EnemyTag)) || (FirstActor->ActorHasTag(PlayerTag) && SecondActor->ActorHasTag(PlayerTag)))
	{
		return true;
	}

	return false;
}

int32 UAuraAbilitySystemLibrary::GetXPRewardForEnemyAndLevel(const UObject* WorldContextObject,
	const EEnemyType EnemyType, const int32 InLevel)
{
	if(InLevel <= 0 || EnemyType == EEnemyType::DefaultMax) return 0;

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	
	if(World && World->GetNetMode() == ENetMode::NM_Client)
	{
		UE_LOG(LogAura, Error, TEXT("Client tried to call %hs, when only the server should."), __FUNCTION__)
		return 0;
	}
	
	const UCurveTable* XPRewardTable = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject))->XPRewardTable;
	if(!XPRewardTable) return 0;
	// Bug: GetDisplayValueAsText will only provide the correct name when not a packaged build. Need to do string manipulation when a packaged build. Already did this in the blaster project.
	const FText EnumText = UEnum::GetDisplayValueAsText(EnemyType);
	//FName DebugName = UEnum::GetValueAsName(EnemyType);
	const FName EnumName = FName(EnumText.ToString());
	const FRealCurve* RewardCurve = XPRewardTable->FindCurve(EnumName, "", true);
	if(!RewardCurve) return 0; //Debug purposes
	const int32 XPReward = static_cast<int32>(RewardCurve->Eval(InLevel));
	
	return XPReward;
}


