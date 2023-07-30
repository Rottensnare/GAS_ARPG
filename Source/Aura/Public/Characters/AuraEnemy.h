// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Characters/AuraCharacterBase.h"
#include "Interfaces/EnemyInterface.h"
#include "Managers/CombatManager.h"
#include "UI/Widget/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

class AAuraAIController;
class UBehaviorTree;
enum class ECharacterClass : uint8;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()


public:

	AAuraEnemy();

	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;

	UPROPERTY(BlueprintReadOnly)
	bool bHighlighted{false};

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Default_Max;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EEnemyType EnemyType;
	
	UFUNCTION()
	void HitReactTagChanged(const FGameplayTag Tag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BaseWalkSpeed = 300.f;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(BlueprintAssignable)
	FOnDeathSignature OnDeathDelegate;
	
	

	/**	Combat AI */
	
	void CombatManagerRegistration(ACombatManager* InCombatManager);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat Manager")
	TObjectPtr<ACombatManager> CombatManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat Manager")
	FSquad CurrentSquad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Manager")
	bool bAutoAssignToSquad = true;

protected:

	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitDefaultAttributes() const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void BeginDestroy() override;
	void OnDeath();

	/**	Enemy level */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	TObjectPtr<UWidgetComponent> HealthBar;

	/**	Category AI	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(VisibleInstanceOnly, Category = "AI")
	TObjectPtr<AAuraAIController> AuraAIController;


private:

	UFUNCTION()
	void CombatManagerInit();

public:

	/**	Category Virtual Overrides */

	virtual int32 GetCharacterLevel() override {return Level;};
	virtual void Die() override;

	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;

	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& AssociatedTag) override;
	virtual EEnemyType GetEnemyType_Implementation() override;
};
