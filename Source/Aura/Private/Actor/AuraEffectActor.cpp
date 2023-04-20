// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetActor.h"


// Sets default values
AAuraEffectActor::AAuraEffectActor()
{

	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));

}


void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if(InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
		return;
	}

	if(DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
		return;
	}

	if(InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if(InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
		return;
	}

	if(DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
		return;
	}

	if(InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	if(InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		RemoveEffectFromTarget(TargetActor);
	}
	
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if(TargetASC == nullptr) return;

	check(GameplayEffectClass)
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle GameplayEffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpecHandle.Data.Get());

	const bool bIsInfiniteEffect = GameplayEffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if(!bIsInfiniteEffect || InfiniteEffectRemovalPolicy != EEffectRemovalPolicy::RemoveOnEndOverlap) return;

	ActiveEffectHandles.Emplace(ActiveEffectHandle, TargetASC);
}

void AAuraEffectActor::RemoveEffectFromTarget(AActor* TargetActor)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if(TargetASC == nullptr) return;

	/*NOTE Leaving this here as a reminder that having the TargetASC as the key doesn't work if you can 
	 *NOTE stack the effects. So having the handle as the key is better.
	if(ActiveEffectHandles.Contains(TargetASC))
	{
		TargetASC->RemoveActiveGameplayEffect(ActiveEffectHandles[TargetASC], 1);
		ActiveEffectHandles.Remove(TargetASC);
	}
	*/

	TArray<FActiveGameplayEffectHandle> HandlesToRemove;

	for(auto& HandlePair : ActiveEffectHandles)
	{
		if(TargetASC == HandlePair.Value)
		{
			TargetASC->RemoveActiveGameplayEffect(HandlePair.Key);
			HandlesToRemove.Add(HandlePair.Key);
		}
	}

	for(auto& Handle : HandlesToRemove)
	{
		ActiveEffectHandles.FindAndRemoveChecked(Handle);
	}
}
