// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interfaces/CombatInterface.h"
#include "AuraCharacterBase.generated.h"


class AAuraGameModeBase;
class ACombatManager;
class UNiagaraSystem;
class UGameplayAbility;
class UAttributeSet;
class UAbilitySystemComponent;
class UCameraComponent;
class USpringArmComponent;
class UGameplayEffect;

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName = NAME_None;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY()
	TObjectPtr<AAuraGameModeBase> AuraGameMode;

	virtual void InitAbilityActorInfo();
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	/**	SET FROM THE DATA ASSET. SETTING THIS DOES NOTHING AT THE MOMENT */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes", meta = (DeprecatedProperty))
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
	
	virtual void InitDefaultAttributes() const;
	//*	Will grant DefaultAbilities to the character */
	void AddCharacterAbilities();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;
	
	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	/**	Time before dissolve effect is started after death. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DissolveDelay = 2.f;

	/**	Time before character is deleted after death. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float LifeTime = 5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Status")
	bool bDead = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TMap<FGameplayTag, FName> TagsToSockets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UNiagaraSystem* BloodEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	USoundBase* DeathSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	USoundAttenuation* BaseSoundAttenuation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MinionCount)
	int32 MinionCount = 0;

	UFUNCTION()
	void OnRep_MinionCount();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	FVector PredictedPosition = FVector::ZeroVector;

	/**	Prediction */
	
	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prediction")
	bool bSaveFrameHistory = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prediction")
	float MaxRecordTime = 2.f;

	void SaveFramePackage();
	void SaveFramePackage(FFramePackage& OutPackage) const;
	
	UFUNCTION(BlueprintCallable)
	void VisualizeFrameHistory();

	UFUNCTION(BlueprintCallable)
	void ExtrapolateFrameHistory(const float ExtrapolationTime = 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prediction")
	float CustomTickRate = 10.f;

	FTimerHandle CustomTickHandle; 

	void CustomTick();

	void StartCustomTick();
	void StopCustomTick();

	void AnalyzeMovementPattern();

	UFUNCTION(BlueprintCallable)
	bool RunningInCircles(const float Threshold, const FVector& CircleCenter);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prediction")
	bool bRunningInCircles = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prediction")
	float LeanDotProduct = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prediction")
	float CircleRadius = 0.f;

private:

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;
	
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> DefaultPassiveAbilities;
	
public:	

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual UAnimMontage* GetAttackMontage_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;

	/**	Detaches weapon and calls Multicast_HandleDeath_Implementation */
	virtual void Die() override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleDeath();
	
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}
	
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override {return BloodEffect;};
	virtual int32 GetMinionCount_Implementation() override {return MinionCount;}
	virtual void SetMinionCount_Implementation(const int32 NewCount) override {MinionCount = NewCount;}
	virtual FVector GetPredictedPosition_Implementation() override {return PredictedPosition;}
	virtual bool IsRunningInCircles_Implementation(const float Threshold, const FVector& CircleCenter) override;
	virtual FFramePackage GetFramePackage_Implementation(const int32 Index) override;
};
