// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controller/AuraAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AAuraAIController::AAuraAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
}

void AAuraAIController::BeginPlay()
{
	
	
	Super::BeginPlay();
}
