// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"


class ULevelUpInfo;
class UAttributeSet;
class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32)

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AAuraPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FOnPlayerStatChanged OnXPChangedDelegate;
	FOnPlayerStatChanged OnLevelChangedDelegate;
	FOnPlayerStatChanged OnAttributePointsChangedDelegate;
	FOnPlayerStatChanged OnSpellPointsChangedDelegate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
	
	void AddToLevel(const int32 InLevel);

protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Level, meta = (AllowPrivateAccess = "true"))
	int32 Level = 1;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	void SetLevel(const int32 InLevel);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_XP, meta = (AllowPrivateAccess = "true"))
	int32 XP = 0;

	UFUNCTION()
	void OnRep_XP(int32 OldXP);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_AttributePoints, meta = (AllowPrivateAccess = "true"))
	int32 AttributePoints = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_SpellPoints, meta = (AllowPrivateAccess = "true"))
	int32 SpellPoints = 0;

	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);
	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints);

public:

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}

	FORCEINLINE int32 GetCharacterLevel() const {return Level;}
	FORCEINLINE int32 GetXP() const {return XP;}
	FORCEINLINE int32 GetAttributePoints() const {return AttributePoints;}
	FORCEINLINE void AddToAttributePoints(const int32 InPoints) {AttributePoints += InPoints; OnAttributePointsChangedDelegate.Broadcast(AttributePoints);}
	FORCEINLINE int32 GetSpellPoints() const {return SpellPoints;}
	FORCEINLINE void AddToSpellPoints(const int32 InPoints) {SpellPoints += InPoints; OnSpellPointsChangedDelegate.Broadcast(SpellPoints);}
	
	void AddToXP(const int32 InXP);
	void SetXP(const int32 InXP);
	
};
