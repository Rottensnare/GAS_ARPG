// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_FireArea.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSetBase.h"
#include "Interfaces/CombatInterface.h"

struct FAuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance)

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;

	FAuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSetBase, FireResistance, Target, false);
		TagsToCaptureDefs.Emplace(FAuraGameplayTags::Get().Attributes_Resistance_Fire, FireResistanceDef);
	}
};

static const FAuraDamageStatics& DamageStatics()
{
	static FAuraDamageStatics DamageStatics;
	return DamageStatics;
}

UExecCalc_FireArea::UExecCalc_FireArea()
{
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
}

void UExecCalc_FireArea::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	if(SourceCombatInterface == nullptr || TargetCombatInterface == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecCalc_Damage:	SourceCombatInterface or TargetCombatInterface is nullptr"))
		return;
	}

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	
	float Damage = 0.f;
	const FGameplayTag DamageType = FAuraGameplayTags::Get().Damage_Fire;
	const FGameplayTag ResistanceType = FAuraGameplayTags::Get().Attributes_Resistance_Fire;
	const FGameplayEffectAttributeCaptureDefinition CaptureDef = FAuraDamageStatics().TagsToCaptureDefs[ResistanceType];
	float DamageTypeValue = Spec.GetModifierMagnitude(0, false);
	float Resistance = 0.f;
	if(ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParameters, Resistance))
	{
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

				
		DamageTypeValue *= (100.f - Resistance) / 100.f;
		
		Damage += DamageTypeValue;
	}
	FGameplayEffectContextHandle GEContextHandle = Spec.GetContext();
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSetBase::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
