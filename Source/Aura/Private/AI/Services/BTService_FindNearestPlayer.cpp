// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTService_FindNearestPlayer.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Characters/AuraCharacter.h"
#include "Kismet/GameplayStatics.h"

UBTService_FindNearestPlayer::UBTService_FindNearestPlayer()
{
	
}

void UBTService_FindNearestPlayer::OnNodeCreated()
{
	TargetToFollowSelector.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_FindNearestPlayer, TargetToFollowSelector), AActor::StaticClass());
	DistanceToTargetSelector.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_FindNearestPlayer, DistanceToTargetSelector));
}

void UBTService_FindNearestPlayer::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
}

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);


	AActor* ClosestActor = nullptr;
	TArray<AActor*> OutActors;
	//UGameplayStatics::GetAllActorsOfClass(AIOwner->GetCharacter(), AAuraCharacter::StaticClass(), OutActors);
	UGameplayStatics::GetAllActorsWithTag(AIOwner->GetCharacter(), FName("Player"), OutActors);

	//TODO: Use delegates to broadcast death and make AI react to it, instead of constantly checking every few milliseconds.
	TArray<AActor*> ActorsToRemove;
	for(AActor* OutActor : OutActors)
	{
		if(ICombatInterface::Execute_IsDead(OutActor))
		{
			ActorsToRemove.Add(OutActor);
		}
	}
	for(AActor* ActorToRemove : ActorsToRemove)
	{
		OutActors.Remove(ActorToRemove);
	}
	
	float ClosestDistance = TNumericLimits<float>::Max();
	for(AActor* TempActor : OutActors)
	{
		//IsValid also checks if Pending kill
		if(!IsValid(TempActor) || !IsValid(AIOwner->GetPawn())) continue;
		const float Distance = FVector::Distance(AIOwner->GetPawn()->GetActorLocation(), TempActor->GetActorLocation());
		if(Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestActor = TempActor;
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetToFollowSelector.SelectedKeyName, ClosestActor);
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(DistanceToTargetSelector.SelectedKeyName, ClosestDistance);
}
