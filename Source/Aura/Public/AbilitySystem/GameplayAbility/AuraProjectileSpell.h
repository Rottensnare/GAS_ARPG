// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility_Damage.h"
#include "AuraProjectileSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraProjectileSpell : public UAuraGameplayAbility_Damage
{
	GENERATED_BODY()

public:


protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AAuraProjectile> ProjectileClass;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(const FVector& TargetLocation);
	
	/**	Uses SuggestProjectileVelocity_CustomArc
	 *	@param TargetLocation Location of the target
	 *	@param SocketTag Tag for the socket from where the projectile is spawned from.
	 *	@param ArcModifier Controls the curve of the projectile arc, where 1.0f is straight towards and 0.f is straight upwards.
	 */
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectileWithArc(const FVector& TargetLocation, const FGameplayTag SocketTag, const float ArcModifier = 0.5f);
	
	/**	Uses pre-calculated LaunchVelocity to spawn a projectile with the said LaunchVelocity.
	 *	NOTE: Should use UDebugFunctionLibrary::PredictProjectileDirection for projectile direction prediction.
	 *	@param SocketTag Tag for the socket where the projectile is spawned from.
	 *	@param LaunchVelocity Precalculated velocity for the projectile.
	 */
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectileWithArc_Predicted(const FGameplayTag SocketTag, const FVector& LaunchVelocity);
};
