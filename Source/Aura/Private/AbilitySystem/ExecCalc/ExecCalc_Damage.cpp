// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
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
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritResist);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance)
	
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	
	AuraDamageStatics()
	{
		double ThisTime = 0;
		{
			SCOPE_SECONDS_COUNTER(ThisTime)
			DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSetBase, Armor, Target, false);
			DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSetBase, BlockChance, Target, false);
			DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSetBase, ArmorPen, Source, false);
			DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSetBase, CritChance, Source, false);
			DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSetBase, CritDamage, Source, false);
			DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSetBase, CritResist, Target, false);
			DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSetBase, ArcaneResistance, Target, false);
			DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSetBase, FireResistance, Target, false);
			DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSetBase, LightningResistance, Target, false);
			DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSetBase, PhysicalResistance, Target, false);
	
			TagsToCaptureDefs.Emplace(FAuraGameplayTags::Get().Attribute_Secondary_Armor, ArmorDef);
			TagsToCaptureDefs.Emplace(FAuraGameplayTags::Get().Attribute_Secondary_BlockChance, BlockChanceDef);
			TagsToCaptureDefs.Emplace(FAuraGameplayTags::Get().Attribute_Secondary_ArmorPenetration, ArmorPenDef);
			TagsToCaptureDefs.Emplace(FAuraGameplayTags::Get().Attribute_Secondary_CriticalHitChance, CritChanceDef);
			TagsToCaptureDefs.Emplace(FAuraGameplayTags::Get().Attribute_Secondary_CriticalHitDamage, CritDamageDef);
			TagsToCaptureDefs.Emplace(FAuraGameplayTags::Get().Attribute_Secondary_CriticalHitResistance, CritResistDef);
			TagsToCaptureDefs.Emplace(FAuraGameplayTags::Get().Attributes_Resistance_Arcane, ArcaneResistanceDef);
			TagsToCaptureDefs.Emplace(FAuraGameplayTags::Get().Attributes_Resistance_Fire, FireResistanceDef);
			TagsToCaptureDefs.Emplace(FAuraGameplayTags::Get().Attributes_Resistance_Lightning, LightningResistanceDef);
			TagsToCaptureDefs.Emplace(FAuraGameplayTags::Get().Attributes_Resistance_Physical, PhysicalResistanceDef);
		}
		const double MilliSeconds = ThisTime * 1000;
		//UE_LOG(LogTemp, Log, TEXT("AuraDamageStatics::Constructor %.2f ms"), MilliSeconds)
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
	RelevantAttributesToCapture.Add(DamageStatics().CritChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritResistDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	double ExecutionTime = 0;
	{
		SCOPE_SECONDS_COUNTER(ExecutionTime)
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
		//NOTE: TODO: This can be made way more efficient. Why are we looping through all the Damage Types and not just the ones that are being used?
		for(const TTuple<FGameplayTag, FGameplayTag>& Pair : FAuraGameplayTags::Get().DamageTypesToResistances)
		{
			const FGameplayTag DamageType = Pair.Key;
			const FGameplayTag ResistanceType = Pair.Value;
			checkf(AuraDamageStatics().TagsToCaptureDefs.Contains(ResistanceType), TEXT("TagsToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceType.ToString());
			const FGameplayEffectAttributeCaptureDefinition CaptureDef = AuraDamageStatics().TagsToCaptureDefs[ResistanceType];
			
			float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageType, false);
			
			float Resistance = 0.f;
			if(ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParameters, Resistance))
			{
				Resistance = FMath::Clamp(Resistance, 0.f, 100.f);
				DamageTypeValue *= (100.f - Resistance) / 100.f;
		
				Damage += DamageTypeValue;
			}
		}
	
		float TargetBlockChance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParameters, TargetBlockChance);
		TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);
		const bool bBlocked = FMath::RandRange(0, 100) < TargetBlockChance;
		/**	Roll 0-100 for checking if target blocked the attack */
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
		const FRealCurve* CritResistCurve = CharacterClassInfo->DamageCalcCoefficients->FindCurve(FName("CritResist"), FString());
	
		if(ArmorPenCurve == nullptr || EffectiveArmorCurve == nullptr || CritResistCurve == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("ExecCalc_Damage:	ArmorPenCurve or EffectiveArmorCurve or CritResistCurve is nullptr"))
			return;
		}
	
		const float ArmorPenCoefficient = ArmorPenCurve->Eval(SourceCombatInterface->GetCharacterLevel());
		const float EffectArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetCharacterLevel());
		const float CritResistCoefficient = CritResistCurve->Eval(TargetCombatInterface->GetCharacterLevel());

		const float EffectiveArmor = FMath::Max<float>((TargetArmor * (100 - SourceArmorPen * ArmorPenCoefficient) / 100.f), 0.f);
		Damage *= (100 - EffectiveArmor * EffectArmorCoefficient) / 100.f;

		float SourceCritChance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritChanceDef, EvaluateParameters, SourceCritChance);
		SourceCritChance = FMath::Max<float>(0.f, SourceCritChance);

		float SourceCritDamage = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritDamageDef, EvaluateParameters, SourceCritDamage);
		SourceCritDamage = FMath::Max<float>(0.f, SourceCritDamage);

		float TargetCritResist = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritResistDef, EvaluateParameters, TargetCritResist);
		TargetCritResist = FMath::Max<float>(0.f, TargetCritResist);

		const float EffectCritChance = SourceCritChance - TargetCritResist * CritResistCoefficient;
		const bool bCriticalHit = FMath::RandRange(0, 100) < EffectCritChance;
		
		/**	Roll 0-100 for checking if the target got critically hit */
		if(bCriticalHit)
		{
			Damage += SourceCritDamage;
		}

		FGameplayEffectContextHandle GEContextHandle = Spec.GetContext();
		UAuraAbilitySystemLibrary::SetIsBlockedHit(GEContextHandle, bBlocked);
		UAuraAbilitySystemLibrary::SetIsCriticalHit(GEContextHandle, bCriticalHit);
	
		const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSetBase::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
		OutExecutionOutput.AddOutputModifier(EvaluatedData);
	}
	double MilliSeconds = ExecutionTime * 1000;
	//UE_LOG(LogTemp, Log, TEXT("ExecCalc_Damage Time To Execute: %.2f ms"), MilliSeconds)
	
}
