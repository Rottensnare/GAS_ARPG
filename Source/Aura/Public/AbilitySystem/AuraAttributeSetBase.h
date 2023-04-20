// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GameplayEffectExtension.h"
#include "AuraAttributeSetBase.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT(BlueprintType)
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties(){};

	UPROPERTY(BlueprintReadOnly, Category = "Effect Properties")
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Effect Properties")
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Effect Properties")
	AController* TargetController = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Effect Properties")
	ACharacter* TargetCharacter = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Effect Properties")
	FGameplayEffectContextHandle EffectContextHandle;
	
	UPROPERTY(BlueprintReadOnly, Category = "Effect Properties")
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Effect Properties")
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Effect Properties")
	AController* SourceController = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Effect Properties")
	ACharacter* SourceCharacter = nullptr;
	
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:

	UAuraAttributeSetBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSetBase, Health);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSetBase, MaxHealth);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSetBase, Mana);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSetBase, MaxMana);

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

private:

	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
};
