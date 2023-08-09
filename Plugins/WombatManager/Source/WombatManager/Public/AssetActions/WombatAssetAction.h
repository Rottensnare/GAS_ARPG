// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "WombatAssetAction.generated.h"

/**
 * 
 */
UCLASS()
class WOMBATMANAGER_API UWombatAssetAction : public UAssetActionUtility
{
	GENERATED_BODY()

public:

	UFUNCTION(CallInEditor)
	void Debug();

	
};
