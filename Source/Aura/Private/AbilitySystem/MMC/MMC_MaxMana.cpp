// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MMC/MMC_MaxMana.h"

#include "AbilitySystem/AuraAttributeSetBase.h"
#include "Interfaces/CombatInterface.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	IntelligenceDef.AttributeToCapture = UAuraAttributeSetBase::GetIntelligenceAttribute();
	IntelligenceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntelligenceDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(IntelligenceDef);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float Intelligence = 0.f;
	GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluateParameters, Intelligence);

	Intelligence = FMath::Max<float>(Intelligence, 0.f);
	
	// ReSharper disable once CppTooWideScope
	if(Spec.GetContext().GetSourceObject() && Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		const int32 CharacterLevel = ICombatInterface::Execute_GetCharacterLevel(Spec.GetContext().GetSourceObject());
		return 100.f + 3.f * Intelligence + 10.f * CharacterLevel;
	}

	UE_LOG(LogTemp, Error, TEXT("Spec source object does not implement the ICombatInterface"))
	return 0.f;
}
