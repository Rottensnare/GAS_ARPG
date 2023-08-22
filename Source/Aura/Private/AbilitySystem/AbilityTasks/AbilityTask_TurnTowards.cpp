// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_TurnTowards.h"

#include "Debug/DebugFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UAbilityTask_TurnTowards::UAbilityTask_TurnTowards(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bSimulatedTask = true;
}

UAbilityTask_TurnTowards* UAbilityTask_TurnTowards::CreateAbilityTask_TurnTowards(UGameplayAbility* OwningAbility,
                                                                                  ACharacter* ActorToTurn,
                                                                                  const float RotationRate,
                                                                                  const FVector TargetLocation,
                                                                                  AActor* InTargetActor)
{
	//TODO: Handle the case where both TargetLocation and InTargetActor are not set
	UAbilityTask_TurnTowards* NewObj = NewAbilityTask<UAbilityTask_TurnTowards>(OwningAbility);
	NewObj->ActorToTurn = ActorToTurn;
	NewObj->RotationRate = RotationRate;
	NewObj->TargetLocation = TargetLocation;
	NewObj->TargetActor = InTargetActor;
	UDebugFunctionLibrary::DebugBoxSimple_Red(ActorToTurn, TargetLocation);
	return  NewObj;
}

void UAbilityTask_TurnTowards::Activate()
{
	
}

void UAbilityTask_TurnTowards::TickTask(float DeltaTime)
{
	if (bFinished)
	{
		return;
	}

	if (!IsValid(ActorToTurn))
	{
		OnFailed.Broadcast();
		bFinished = true;
		return;
	}
	
	Super::TickTask(DeltaTime);

	
	FVector ActorToTarget;
	if(IsValid(TargetActor))
	{
		//TODO: Should be changed so that it takes the movement prediction into consideration 
		ActorToTarget = TargetActor->GetActorLocation() - ActorToTurn->GetActorLocation();
	}
	else
	{
		ActorToTarget = TargetLocation - ActorToTurn->GetActorLocation();
	}
	
	ActorToTarget.Z = 0.0f;  // Ignore vertical component

	FRotator DesiredRotation = ActorToTarget.ToOrientationRotator();
	DesiredRotation.Pitch = 0.0f;  // Set pitch to zero

	// Calculate rotation difference
	const FRotator RotationChange = DesiredRotation - ActorToTurn->GetActorRotation();
	
	if (FMath::Abs(RotationChange.Yaw) < YawRotationTolerance)
	{
		// Rotation completed
		bFinished = true;
		OnFinishedTurning.Broadcast();
		EndTask();
		return;
	}

	// We don't want to turn too much
	RotationRate = FMath::Min(FMath::Abs(RotationChange.GetNormalized().Yaw), RotationRate*DeltaTime);
	
	// Apply the rotation change
	if(RotationChange.GetNormalized().Yaw <= 0.f)
	{
		ActorToTurn->AddActorLocalRotation(FRotator(0.f, -RotationRate, 0.f) * DeltaTime);
	}
	else
	{
		ActorToTurn->AddActorLocalRotation(FRotator(0.f, RotationRate, 0.f) * DeltaTime);
	}
	
	
}
