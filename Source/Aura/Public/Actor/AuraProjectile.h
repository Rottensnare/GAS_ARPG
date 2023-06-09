// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "AuraProjectile.generated.h"

class UProjectileMovementComponent;
class UNiagaraSystem;
UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraProjectile();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;
	
protected:

	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY(EditAnywhere)
	float ProjectileLifeSpan = 10.f;

	

	/**	Effects */
	
	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<USoundBase> LoopingSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopSoundComponent;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<USoundAttenuation> ImpactSoundAttenuation;

	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AdvancedDisplay))
	float VolumeMultiplier = 1.f;


private:

	bool bHit = false;
};
