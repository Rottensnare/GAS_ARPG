// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTService_FindNearestPlayer.h"

#include "AIController.h"
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

	float ClosestDistance = TNumericLimits<float>::Max();
	for(AActor* TempActor : OutActors)
	{
		//IsValid also checks if Pending kill
		if(!IsValid(TempActor) || !IsValid(AIOwner->GetPawn())) continue;
		
		if(const float Distance = FVector::Distance(AIOwner->GetPawn()->GetActorLocation(), TempActor->GetActorLocation()) < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestActor = TempActor;
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetToFollowSelector.SelectedKeyName, ClosestActor);
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(DistanceToTargetSelector.SelectedKeyName, ClosestDistance);
}
