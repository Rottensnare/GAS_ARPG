// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/EnemyInterface.h"
#include "UI/Widget/DamageTextComponent.h"
#include "AuraPlayerController.generated.h"

class USplineComponent;
class UAuraAbilitySystemComponentBase;
struct FGameplayTag;
class UAuraInputConfig;
class IEnemyInterface;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()


public:

	AAuraPlayerController();
	void AutoRun();
	virtual void PlayerTick(float DeltaTime) override;
	
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(const float Damage, AActor* TargetActor);
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FVector FloatingTextOffset = FVector::ZeroVector;
	
protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	

private:

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;

	void ShiftPressed(){bShiftKeyDown = true;}
	void ShiftReleased(){bShiftKeyDown = false;}
	bool bShiftKeyDown = false;

	void Move(const FInputActionValue& InputActionValue);

	FHitResult CursorHitResult;
	void CursorTrace();
	IEnemyInterface* LastActor;
	IEnemyInterface* CurrentActor;
	

	UPROPERTY()
	APawn* ControlledPawn;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponentBase> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponentBase* GetASC();

	/** Category Click to Move */
	
	FVector CachedDestination{FVector::ZeroVector};
	float FollowTime{0.f};
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float ShortPressThreshold{0.2f};
	
	bool bAutoRunning = false;
	bool bTargeting = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> SplineComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextCompClass;
};
