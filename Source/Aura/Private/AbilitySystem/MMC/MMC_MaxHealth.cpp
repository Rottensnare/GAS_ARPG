// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MMC/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSetBase.h"
#include "Interfaces/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDef.AttributeToCapture = UAuraAttributeSetBase::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(VigorDef);
	
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluateParameters, Vigor);

	Vigor = FMath::Max<float>(Vigor, 0.f);

	// ReSharper disable once CppTooWideScope
	if(Spec.GetContext().GetSourceObject() && Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		const int32 CharacterLevel = ICombatInterface::Execute_GetCharacterLevel(Spec.GetContext().GetSourceObject());
		return 20.f + 2.5f * Vigor + 5.f * CharacterLevel;
	}

	UE_LOG(LogTemp, Error, TEXT("Spec source object does not implement the ICombatInterface"))
	return 0.f;
	
}
