// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_TurnTowards.h"

#include "Aura/AuraLogChannels.h"
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
                                                                                  ACharacter* ActorToTurn, const FVector& TargetLocation)
{
	Count++;
	UE_LOG(LogAura, Warning, TEXT("Count: %d"), Count)
	UAbilityTask_TurnTowards* NewObj = NewAbilityTask<UAbilityTask_TurnTowards>(OwningAbility);
	NewObj->ActorToTurn = ActorToTurn;
	NewObj->TargetLocation = TargetLocation;
	UDebugFunctionLibrary::DebugBoxSimple_Red(ActorToTurn, TargetLocation);
	return  NewObj;
}

void UAbilityTask_TurnTowards::Activate()
{
	
}

/*void UAbilityTask_TurnTowards::TickTask(float DeltaTime)
{
	if(bFinished) return;
	if(!IsValid(ActorToTurn))
	{
		OnFailed.Broadcast();
		bFinished = true;
		return;
	}
	
	Super::TickTask(DeltaTime);

	// Calculate the rotation change for this frame
	FRotator RotationChange = FRotator(ActorToTurn->GetCharacterMovement()->RotationRate * DeltaTime);

	// Apply the rotation change
	const FVector ActorToTarget = TargetLocation - ActorToTurn->GetActorLocation();
	const float DotProduct = FVector::DotProduct(ActorToTarget.GetSafeNormal(), ActorToTurn->GetActorForwardVector());
	if( FMath::Acos(DotProduct) < 0.5f)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			bFinished = true;
			OnFinishedTurning.Broadcast();
		}
		EndTask();
	}
	
	const FVector CrossProduct = FVector::CrossProduct(ActorToTurn->GetActorForwardVector(), FVector(0.f, 0.f, 1.f));
	if(FVector::DotProduct(CrossProduct.GetSafeNormal(), ActorToTarget.GetSafeNormal()) > 0.f)
	{
		ActorToTurn->AddActorLocalRotation(FRotator(0.f, -RotationChange.Yaw, 0.f));
	}
	else
	{
		ActorToTurn->AddActorLocalRotation(FRotator(0.f, RotationChange.Yaw, 0.f));
	}
}
*/
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

	FVector ActorToTarget = TargetLocation - ActorToTurn->GetActorLocation();
	ActorToTarget.Z = 0.0f;  // Ignore vertical component

	FRotator DesiredRotation = ActorToTarget.ToOrientationRotator();
	DesiredRotation.Pitch = 0.0f;  // Set pitch to zero

	// Calculate rotation change
	FRotator RotationChange = DesiredRotation - ActorToTurn->GetActorRotation();

	if (RotationChange.Yaw < 10.f)
	{
		// Rotation completed
		bFinished = true;
		OnFinishedTurning.Broadcast();
		EndTask();
		return;
	}

	// Limit rotation change
	RotationChange = RotationChange.GetNormalized();

	// Apply the rotation change
	ActorToTurn->AddActorLocalRotation(RotationChange * DeltaTime);
}
