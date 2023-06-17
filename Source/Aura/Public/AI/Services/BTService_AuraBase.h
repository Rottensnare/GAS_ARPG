// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BTService_AuraBase.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UBTService_AuraBase : public UBTService_BlueprintBase
{
	GENERATED_BODY()

protected:

	virtual void OnNodeCreated() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<UClass*, FBlackboardKeySelector> KeySelectorFilter;
};
