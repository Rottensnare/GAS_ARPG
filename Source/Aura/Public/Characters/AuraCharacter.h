// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/AuraCharacterBase.h"
#include "Interfaces/PlayerInterface.h"
#include "AuraCharacter.generated.h"

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

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

public:
	
	/**	Category Virtual Overrides */

	virtual int32 GetCharacterLevel() override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& AssociatedTag) override;
	virtual FFramePackage GetFramePackage_Implementation(const int32 Index) override;
	virtual void AddToXP_Implementation(const int32 InXP) override;
};
