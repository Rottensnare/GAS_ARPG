// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/AuraCharacterBase.h"
#include "Interfaces/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:

	AAuraCharacter();
	virtual void InitAbilityActorInfo() override;

protected:

	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> MainCamera;

	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Up")
	TObjectPtr<UNiagaraComponent> LevelUpEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Up")
	TObjectPtr<USoundBase> LevelUpSound;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastLevelUpParticles();

public:
	
	/**	Category Virtual Overrides */

	virtual int32 GetCharacterLevel_Implementation() override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(const int32 InXP) override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& AssociatedTag) override;
	virtual FFramePackage GetFramePackage_Implementation(const int32 Index) override;
	virtual void AddToXP_Implementation(const int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetAttributePointsReward_Implementation(const int32 InLevel) const override;
	virtual int32 GetSpellPointsReward_Implementation(const int32 InLevel) const override;
	virtual void AddToPlayerLevel_Implementation(const int32 InLevel) override;
	virtual void AddToAttributePoints_Implementation(const int32 InAttributePoints) override;
	virtual void AddToSpellPoints_Implementation(const int32 InSpellPoints) override;
};
