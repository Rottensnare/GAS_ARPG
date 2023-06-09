// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSetBase.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interfaces/CombatInterface.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPen);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSetBase, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSetBase, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSetBase, ArmorPen, Source, false);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DamageStatics;
	return DamageStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
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

	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);
	
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);
	
	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	if(bBlocked)
	{
		Damage *= 0.5f;
	}

	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);

	float SourceArmorPen = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenDef, EvaluateParameters, SourceArmorPen);
	SourceArmorPen = FMath::Max<float>(SourceArmorPen, 0.f);

	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	if(CharacterClassInfo == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecCalc_Damage:	CharacterClassInfo is nullptr"))
		return;
	}
	
	const FRealCurve* ArmorPenCurve = CharacterClassInfo->DamageCalcCoefficients->FindCurve(FName("ArmorPen"), FString());
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalcCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	if(ArmorPenCurve == nullptr || EffectiveArmorCurve == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ExecCalc_Damage:	ArmorPenCurve or EffectiveArmorCurve is nullptr"))
		return;
	}

	const int32 EnemyLevelDEBUG = TargetCombatInterface->GetCharacterLevel();
	
	const float ArmorPenCoefficient = ArmorPenCurve->Eval(SourceCombatInterface->GetCharacterLevel());
	const float EffectArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetCharacterLevel());

	const float EffectiveArmor = FMath::Max<float>((TargetArmor * (100 - SourceArmorPen * ArmorPenCoefficient) / 100.f), 0.f);
	Damage *= (100 - EffectiveArmor * EffectArmorCoefficient) / 100.f;
	
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSetBase::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
	
}
