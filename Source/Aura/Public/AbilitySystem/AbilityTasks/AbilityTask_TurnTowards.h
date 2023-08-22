// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_TurnTowards.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishedTurning);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFailed);

/**
 * 
 */
UCLASS()
class AURA_API UAbilityTask_TurnTowards : public UAbilityTask
{
	GENERATED_BODY()

public:

	UAbilityTask_TurnTowards(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable)
	FOnFinishedTurning OnFinishedTurning;

	//Fails if actor to turn is nullptr
	UPROPERTY(BlueprintAssignable)
	FOnFailed OnFailed;

	/**	ActorToTurn will turn towards TargetLocation if InTargetActor is not set
	 *	@param OwningAbility GameplayAbility that owns this task
	 *	@param ActorToTurn Actor that will be turned towards the target
	 *	@param RotationRate Yaw Rotation rate of the actor. Should be taken from CharacterMovement->RotationRate.Yaw
	 *	@param TargetLocation Target location that the actor tries to turn towards, but only if InTargetActor is not set.
	 *	@param InTargetActor Target actor that the actor tries to turn towards
	 *
	 *	@returns an UAbilityTask_TurnTowards* 
	 **/
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "TurnTowards", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_TurnTowards* CreateAbilityTask_TurnTowards(UGameplayAbility* OwningAbility, ACharacter* ActorToTurn, const float RotationRate, const FVector TargetLocation = FVector::ZeroVector, AActor* InTargetActor = nullptr);

	// Defines the acceptable angle range, in degrees, for considering rotation sufficiently aligned with the desired orientation. 
	float YawRotationTolerance = 15.f;
	
private:

	TObjectPtr<AActor> ActorToTurn;
	float RotationRate;
	FVector TargetLocation;
	TObjectPtr<AActor> TargetActor;
	
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

	bool bFinished = false;
};
