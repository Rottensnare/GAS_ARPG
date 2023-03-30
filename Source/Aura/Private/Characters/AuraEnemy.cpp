// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AuraEnemy.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AAuraEnemy::HighlightActor()
{
	//UE_LOG(LogTemp, Display, TEXT("HighlightActor Actor called on actor: %s"), *GetName())
	
	check(Weapon)
	check(GetMesh())
	
	bHighlighted = true;
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
}

void AAuraEnemy::UnHighlightActor()
{
	//UE_LOG(LogTemp, Display, TEXT("UnHighlightActor Actor called on actor: %s"), *GetName())
	
	check(Weapon)
	check(GetMesh())
	
	bHighlighted = false;
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}
