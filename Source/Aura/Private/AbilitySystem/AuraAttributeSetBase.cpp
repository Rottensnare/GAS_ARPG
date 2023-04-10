// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAttributeSetBase.h"

#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

UAuraAttributeSetBase::UAuraAttributeSetBase()
{
	InitHealth(25.f);
	InitMaxHealth(100.f);
	InitMana(75.f);
	InitMaxMana(100.f);
}

void UAuraAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, MaxMana, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, Health, OldHealth);
}

void UAuraAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSetBase::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, Mana, OldMana);
}

void UAuraAttributeSetBase::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, MaxMana, OldMaxMana);
}
