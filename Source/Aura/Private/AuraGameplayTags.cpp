// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{

	/**	Category Primary */
	
	GameplayTags.Attribute_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"), FString("Increases physical damage."));
	GameplayTags.Attribute_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"), FString("Increases magical damage."));
	GameplayTags.Attribute_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Primary.Resilience"), FString("Increases Armor and Armor Penetration."));
	GameplayTags.Attribute_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"), FString("Increases Health."));

	/**	Category Secondary */
	
	GameplayTags.Attribute_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"), FString("Reduces damage taken & improves Block Chance."));
	GameplayTags.Attribute_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"), FString("Ignored percentage of enemy Armor, increases Critical Hit Chance."));
	GameplayTags.Attribute_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.BlockChance"), FString("Chance to cut incoming damage to half."));
	GameplayTags.Attribute_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"), FString("Chance to double damage plus Critical Hit bonus."));
	GameplayTags.Attribute_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamage"), FString("Bonus damage added when a Critical Hit is scored."));
	GameplayTags.Attribute_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitResistance"), FString("Reduces Critical Hit Chance of attacking enemies."));
	GameplayTags.Attribute_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.HealthRegeneration"), FString("Amount of Health regenerated every 1 second."));
	GameplayTags.Attribute_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ManaRegeneration"), FString("Amount of Mana regenerated every 1 second."));
	GameplayTags.Attribute_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"), FString("Maximum amount of Health."));
	GameplayTags.Attribute_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxMana"), FString("Maximum amount of Mana."));

	/**	Category Meta */

	GameplayTags.Attribute_Meta_IncomingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Meta.IncomingXP"), FString("Incoming XP."));
	
	/**	Category Caller */
	
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage"), FString("Damaaj"));

	/**	Category Damage types */

	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Arcane"), FString("Arcane Damage Type"));
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Fire"), FString("Fire Damage Type"));
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Lightning"), FString("Lightning Damage Type"));
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Physical"), FString("Physical Damage Type"));

	/**	Category Resistance types */
	
	GameplayTags.Attributes_Resistance_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Arcane"), FString("Arcane Resistance Type"));
	GameplayTags.Attributes_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Fire"), FString("Fire Resistance Type"));
	GameplayTags.Attributes_Resistance_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Lightning"), FString("Lightning Resistance Type"));
	GameplayTags.Attributes_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Physical"), FString("Physical Resistance Type"));
	
	GameplayTags.DamageTypesToResistances.Emplace(GameplayTags.Damage_Arcane, GameplayTags.Attributes_Resistance_Arcane);
	GameplayTags.DamageTypesToResistances.Emplace(GameplayTags.Damage_Fire, GameplayTags.Attributes_Resistance_Fire);
	GameplayTags.DamageTypesToResistances.Emplace(GameplayTags.Damage_Lightning, GameplayTags.Attributes_Resistance_Lightning);
	GameplayTags.DamageTypesToResistances.Emplace(GameplayTags.Damage_Physical, GameplayTags.Attributes_Resistance_Physical);

	/**	Category Sockets */
	
	
	GameplayTags.CombatSocket_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.Weapon"), FString("Weapon Attack Montage"));
	GameplayTags.CombatSocket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.LeftHand"), FString("Left Hand Attack Montage"));
	GameplayTags.CombatSocket_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.RightHand"), FString("Right Hand Attack Montage"));
	GameplayTags.CombatSocket_Tail = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.Tail"), FString("Tail Attack Montage"));
	
	/**	Category Montages */
	
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Effects.HitReact"), FString("Hit Reaction"));
	GameplayTags.Montage_Attack_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.1"), FString("Montage Attack 1"));
	GameplayTags.Montage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.2"), FString("Montage Attack 2"));
	GameplayTags.Montage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.3"), FString("Montage Attack 3"));
	GameplayTags.Montage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.4"), FString("Montage Attack 4"));
	
	/** Category Status */

	GameplayTags.Status_CC_Stunned = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Status.CC.Stunned"), FString("Character is Stunned"));
	GameplayTags.Status_Dead = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Status.Dead"), FString("Character is dead"));
	
	/** Category Abilities */

	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Attack"), FString("Tag for attack abilities"));
	GameplayTags.Abilities_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Summon"), FString("Tag for summon abilities"));
	GameplayTags.Abilities_Fire_Fireball = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Fire.Fireball"), FString("Tag for Fireball abilities"));
	GameplayTags.Abilities_Lightning_Electrocute = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Lightning.Electrocute"), FString("Tag for Electrocute abilities"));
	GameplayTags.Abilities_Physical_SpearThrow = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Physical.SpearThrow"), FString("Tag for SpearThrow abilities"));
	GameplayTags.Abilities_Passive_Sprint = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Passive.Sprint"), FString("Tag for sprint abilities"));

	GameplayTags.Abilities_Status_Locked = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Status.Locked"), FString("Tag for Ability Status Locked"));
	GameplayTags.Abilities_Status_Unlocked = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Status.Unlocked"), FString("Tag for Ability Status Unlocked"));
	GameplayTags.Abilities_Status_Eligible = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Status.Eligible"), FString("Tag for Ability Status Eligible"));
	GameplayTags.Abilities_Status_Equipped = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Status.Equipped"), FString("Tag for Ability Status Equipped"));

	GameplayTags.Abilities_Type_Offensive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Type.Offensive"), FString("Tag for Ability Type Offensive"));
	GameplayTags.Abilities_Type_Passive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Type.Passive"), FString("Tag for Ability Type Passive"));
	GameplayTags.Abilities_Type_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Type.None"), FString("Tag for Ability Type None"));
	
	/** Category Ability Characteristics */

	GameplayTags.Abilities_Range_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Range.Melee"), FString("Tag for melee range ability characteristic"));
	GameplayTags.Abilities_Range_Short = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Range.Short"), FString("Tag for short range ability characteristic"));
	GameplayTags.Abilities_Range_Medium = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Range.Medium"), FString("Tag for medium range ability characteristic"));
	GameplayTags.Abilities_Range_Long = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Range.Long"), FString("Tag for long range ability characteristic"));
	
	/** Category Cooldowns */
	
	GameplayTags.Cooldown_Fire_Fireball = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Cooldown.Fire.Fireball"), FString("Tag for Fireball cooldown"));
	GameplayTags.Cooldown_Physical_SpearThrow = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Cooldown.Physical.SpearThrow"), FString("Tag for SpearThrow cooldown"));
	GameplayTags.Cooldown_Passive_Sprint = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Cooldown.Passive.Sprint"), FString("Tag for Sprint cooldown"));
	
	/**	Category Input */
	
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.LMB"), FString("Input Tag for Left Mouse Button."));
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.RMB"), FString("Input Tag for Right Mouse Button."));
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("InputTag.1"), FString("Input Tag for 1 Button."));
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.2"), FString("Input Tag for 2 Button."));
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("InputTag.3"), FString("Input Tag for 3 Button."));
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.4"), FString("Input Tag for 4 Button."));
	GameplayTags.InputTag_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.5"), FString("Input Tag for 5 Button."));
	GameplayTags.InputTag_Passive_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Passive.1"), FString("Input Tag for passive 1."));
	GameplayTags.InputTag_Passive_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("InputTag.Passive.2"), FString("Input Tag for passive 2."));
	
}