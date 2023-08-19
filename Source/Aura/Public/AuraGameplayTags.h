// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * 
 */

struct FAuraGameplayTags
{
 
public:
 
 static const FAuraGameplayTags& Get(){return GameplayTags;}
 static void InitializeNativeGameplayTags();

 /** Category Attributes */
 
 FGameplayTag Attribute_Primary_Strength;
 FGameplayTag Attribute_Primary_Intelligence;
 FGameplayTag Attribute_Primary_Resilience;
 FGameplayTag Attribute_Primary_Vigor;
 
 FGameplayTag Attribute_Secondary_MaxHealth;
 FGameplayTag Attribute_Secondary_MaxMana;
 
 FGameplayTag Attribute_Secondary_Armor;
 FGameplayTag Attribute_Secondary_ArmorPenetration;
 FGameplayTag Attribute_Secondary_BlockChance;
 FGameplayTag Attribute_Secondary_CriticalHitChance;
 FGameplayTag Attribute_Secondary_CriticalHitDamage;
 FGameplayTag Attribute_Secondary_CriticalHitResistance;
 FGameplayTag Attribute_Secondary_HealthRegeneration;
 FGameplayTag Attribute_Secondary_ManaRegeneration;

 FGameplayTag Attribute_Meta_IncomingXP;

 /** Category Damage types */
 
 FGameplayTag Damage;
 FGameplayTag Damage_Arcane;
 FGameplayTag Damage_Fire;
 FGameplayTag Damage_Lightning;
 FGameplayTag Damage_Physical;

 FGameplayTag Attributes_Resistance_Arcane;
 FGameplayTag Attributes_Resistance_Fire;
 FGameplayTag Attributes_Resistance_Lightning;
 FGameplayTag Attributes_Resistance_Physical;

 TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;

 /** Category Sockets */
 
 FGameplayTag CombatSocket_Weapon;
 FGameplayTag CombatSocket_RightHand;
 FGameplayTag CombatSocket_LeftHand;
 FGameplayTag CombatSocket_Tail;
 
 /** Category Montages */

 FGameplayTag Effects_HitReact;
 FGameplayTag Montage_Attack_1;
 FGameplayTag Montage_Attack_2;
 FGameplayTag Montage_Attack_3;
 FGameplayTag Montage_Attack_4;

 /** Category Status */

 FGameplayTag Status_CC_Stunned;
 FGameplayTag Status_Dead;
 
 /** Category Abilities */

 FGameplayTag Abilities_Attack;
 FGameplayTag Abilities_Summon;
 
 FGameplayTag Abilities_Fire_Fireball;
 FGameplayTag Abilities_Physical_SpearThrow;
 FGameplayTag Abilities_Passive_Sprint;

/** Category Ability Characteristics */
 
 FGameplayTag Abilities_Range_Melee;
 FGameplayTag Abilities_Range_Short;
 FGameplayTag Abilities_Range_Medium;
 FGameplayTag Abilities_Range_Long;

 /** Category Cooldowns */

 FGameplayTag Cooldown_Fire_Fireball;
 FGameplayTag Cooldown_Physical_SpearThrow;
 FGameplayTag Cooldown_Passive_Sprint;
 
 /** Category InputTags */

 FGameplayTag InputTag_LMB;
 FGameplayTag InputTag_RMB;
 FGameplayTag InputTag_1;
 FGameplayTag InputTag_2;
 FGameplayTag InputTag_3;
 FGameplayTag InputTag_4;
 FGameplayTag InputTag_5;
 
protected:

private:

 static FAuraGameplayTags GameplayTags;
};
