// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class UNiagaraSystem;
USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* Montage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag MontageTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SocketTag;
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int32 GetCharacterLevel();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetCombatSocketLocation(const FGameplayTag& AssociatedTag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetAttackMontage();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);
	
	virtual void Die() = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetCombatTarget(AActor* InCombatTarget);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetCombatTarget() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FTaggedMontage> GetAttackMontages();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UNiagaraSystem* GetBloodEffect();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetMinionCount();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetMinionCount(const int32 NewCount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetPredictedPosition();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsRunningInCircles(const float Threshold, const FVector& CircleCenter);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FFramePackage GetFramePackage(const int32 Index);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	EEnemyType GetEnemyType();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetWeaponVisibility(const bool bVisible);
};
