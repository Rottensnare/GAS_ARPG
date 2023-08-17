// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IEnemyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//Note: = 0 means that it is a pure virtual function. No default implementation needed.
	
	virtual void HighlightActor() = 0;
	virtual void UnHighlightActor() = 0;
	//Used TOptional because why not?
	virtual void DebugHighlightActor(TOptional<int32> OptionalStencilValue = TOptional<int32>());
	virtual void DebugUnHighlightActor();
};
