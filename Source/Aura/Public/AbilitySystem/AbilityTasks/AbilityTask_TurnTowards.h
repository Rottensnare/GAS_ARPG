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
	
	UPROPERTY(BlueprintAssignable)
	FOnFailed OnFailed;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "TurnTowards", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_TurnTowards* CreateAbilityTask_TurnTowards(UGameplayAbility* OwningAbility, ACharacter* ActorToTurn, const FVector& TargetLocation);

private:

	TObjectPtr<ACharacter> ActorToTurn;
	FVector TargetLocation;
	
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

	bool bFinished = false;

	static inline int32 Count = 0;
};
