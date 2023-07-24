// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class ACombatManager;
class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<ACombatManager> CombatManagerClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	ACombatManager* CombatManager;

	bool bCombatManagerReady = false;

	virtual void BeginPlay() override;
	
public:

	UFUNCTION(BlueprintCallable)
	ACombatManager* GetCombatManager();

	bool GetCombatManagerReady() const {return bCombatManagerReady;}
	void SetCombatManagerReady(const bool bReady) {bCombatManagerReady = bReady;}
};
