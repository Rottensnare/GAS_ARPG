// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAttributeSetBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/GameplayAbility/AuraGameplayAbility.h"
#include "Aura/AuraLogChannels.h"
#include "GameFramework/Character.h"
#include "Interfaces/CombatInterface.h"
#include "Interfaces/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerController.h"

DECLARE_STATS_GROUP(TEXT("AuraStatGroup"), STATGROUP_AuraStatGroup, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("SoftTargeting - Some Function"), STAT_SoftTargetSomeFunction, STATGROUP_AuraStatGroup);

UAuraAttributeSetBase::UAuraAttributeSetBase()
{
	//InitHealth(25.f);
	//InitMana(75.f);

	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

	// Primary
	TagsToAttributes.Add(GameplayTags.Attribute_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Primary_Vigor, GetVigorAttribute);

	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_ArmorPenetration, GetArmorPenAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_CriticalHitChance, GetCritChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_CriticalHitDamage, GetCritDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_CriticalHitResistance, GetCritResistAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_HealthRegeneration, GetHealthRegenAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_ManaRegeneration, GetManaRegenAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attribute_Secondary_MaxMana, GetMaxManaAttribute);

	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
}

void UAuraAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//*		Vital Attributes	*/
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, MaxMana, COND_None, REPNOTIFY_Always);

	//*		Primary Attributes	*/

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, Vigor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, Strength, COND_None, REPNOTIFY_Always);
	
	//*		Secondary Attributes	*/
	
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, ArmorPen, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, CritChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, CritDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, CritResist, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, HealthRegen, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, ManaRegen, COND_None, REPNOTIFY_Always);

	//*		Resistance Attributes	*/

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSetBase, PhysicalResistance, COND_None, REPNOTIFY_Always);

	
}

void UAuraAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	//Note: Only clamps what returns from querying the modifier. Actual clamping is done in PostGameplayEffectExecute
	
	if(Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxHealth());
	}
	if(Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxMana());
	}
}

void UAuraAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	double ThisTime = 0; //NOTE: Delete this later. It is for testing.
	{
		SCOPE_SECONDS_COUNTER(ThisTime) //NOTE: And This
		FEffectProperties Props;
		SetEffectProperties(Data, Props);
		
		if(Data.EvaluatedData.Attribute == GetHealthAttribute())
		{
			SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		}
		if(Data.EvaluatedData.Attribute == GetManaAttribute())
		{
			SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
		}
		if(Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
		{
			const float LocalIncomingDamage = GetIncomingDamage();
			SetIncomingDamage(0.f);
			if(LocalIncomingDamage > 0.f)
			{
				const float NewHealth = GetHealth() - LocalIncomingDamage;
				SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

				const bool bFatal = NewHealth <= 0.f;
				if(bFatal == false)
				{
					//TODO: need to rework it so that no cast is needed. Too expensive for something that can happen very rapidly.
					const UAuraGameplayAbility* CurrentAbility = Cast<UAuraGameplayAbility>(Props.EffectContextHandle.GetAbility());
					if(CurrentAbility && CurrentAbility->StunChance > FMath::FRand()) //Note: Pretty sure even if StunChance = 0.f, it can still enter, but very very rarely
					{
						FGameplayTagContainer TagContainer;
						TagContainer.AddTag(FAuraGameplayTags::Get().Effects_HitReact);
						Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
					}
				}
				else
				{
					if(ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor))
					{
						CombatInterface->Die();
					}
					SendXPEvent(Props);
				}
				
				ShowFloatingText(Props,
					LocalIncomingDamage,
					UAuraAbilitySystemLibrary::GetEffectModifierBits(Props.EffectContextHandle));

				
			}
		}
		if(Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
		{
			const float LocalIncomingXP = GetIncomingXP();
			SetIncomingXP(0.f);
			UE_LOG(LogAura, Warning, TEXT("Incoming XP = %f"), LocalIncomingXP)
			
			IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
		}
	}
	const double Milliseconds = ThisTime * 1000; //NOTE: And These
	//UE_LOG(LogTemp, Log, TEXT("PostGameplayEffectExecute %.2f ms"), Milliseconds)
}

void UAuraAttributeSetBase::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if(Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if(const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if(Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}


	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}

	
}

void UAuraAttributeSetBase::ShowFloatingText(const FEffectProperties& Props, const float Damage, const int32 EffectModBits) const
{
	//	Don't show damage numbers from damage done to self
	if(Props.SourceCharacter != Props.TargetCharacter)
	{
		if(AAuraPlayerController* APC = Cast<AAuraPlayerController>(Props.SourceController))
		{
			APC->ShowDamageNumber(Damage, Props.TargetCharacter, EffectModBits);
		}
		else if(AAuraPlayerController* APC2 = Cast<AAuraPlayerController>(Props.TargetController))
		{
			APC2->ShowDamageNumber(Damage, Props.TargetCharacter, EffectModBits);
		}
	}
}

void UAuraAttributeSetBase::SendXPEvent(const FEffectProperties& Props) const
{
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetCharacter);
	if(!CombatInterface) return;
	
	const int32 TargetLevel = CombatInterface->GetCharacterLevel();
	const EEnemyType EnemyType = ICombatInterface::Execute_GetEnemyType(Props.TargetCharacter);
	const int32 XPReward = UAuraAbilitySystemLibrary::GetXPRewardForEnemyAndLevel(Props.TargetCharacter, EnemyType, TargetLevel);
	FGameplayEventData Payload;
	Payload.EventTag = FAuraGameplayTags::Get().Attribute_Meta_IncomingXP;
	Payload.EventMagnitude = XPReward;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, Payload.EventTag, Payload);
}

#pragma region  Rep Notifies

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

void UAuraAttributeSetBase::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, Strength, OldStrength);
}

void UAuraAttributeSetBase::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, Intelligence, OldIntelligence);
}

void UAuraAttributeSetBase::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, Resilience, OldResilience);
}

void UAuraAttributeSetBase::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, Vigor, OldVigor);
}

void UAuraAttributeSetBase::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, Armor, OldArmor);
}

void UAuraAttributeSetBase::OnRep_ArmorPen(const FGameplayAttributeData& OldArmorPen) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, ArmorPen, OldArmorPen);
}

void UAuraAttributeSetBase::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, BlockChance, OldBlockChance);
}

void UAuraAttributeSetBase::OnRep_CritChance(const FGameplayAttributeData& OldCritChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, CritChance, OldCritChance);
}

void UAuraAttributeSetBase::OnRep_CritDamage(const FGameplayAttributeData& OldCritDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, CritDamage, OldCritDamage);
}

void UAuraAttributeSetBase::OnRep_CritResist(const FGameplayAttributeData& OldCritResist) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, CritResist, OldCritResist);
}

void UAuraAttributeSetBase::OnRep_HealthRegen(const FGameplayAttributeData& OldHealthRegen) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, HealthRegen, OldHealthRegen);
}

void UAuraAttributeSetBase::OnRep_ManaRegen(const FGameplayAttributeData& OldManaRegen) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, ManaRegen, OldManaRegen);
}

void UAuraAttributeSetBase::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, ArcaneResistance, OldArcaneResistance);
}

void UAuraAttributeSetBase::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, FireResistance, OldFireResistance);
}

void UAuraAttributeSetBase::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, LightningResistance, OldLightningResistance);
}

void UAuraAttributeSetBase::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSetBase, PhysicalResistance, OldPhysicalResistance);
}

#pragma endregion 
