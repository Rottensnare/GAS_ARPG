// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayAbility/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/CombatInterface.h"
#include "Kismet/GameplayStatics.h"


void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	
	
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& TargetLocation)
{
	if(GetAvatarActorFromActorInfo()->HasAuthority())
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), FAuraGameplayTags::Get().CombatSocket_Weapon);
		FRotator Rotation = (TargetLocation - SocketLocation).Rotation();
		Rotation.Pitch = 0.f;
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());
		
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			
		
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);
		Projectile->DamageEffectSpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
		
		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		for(auto& Pair : DamageTypes)
		{
			const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Projectile->DamageEffectSpecHandle, Pair.Key, ScaledDamage);
		}
		Projectile->FinishSpawning(SpawnTransform);
		
	}
}

bool UAuraProjectileSpell::SpawnProjectileWithArc(const FVector& TargetLocation, const FGameplayTag SocketTag, const float ArcModifier)
{
	if(GetAvatarActorFromActorInfo()->HasAuthority())
	{
		// Get correct socket location from where projectile is spawned from
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);

		// Spawn Deferred, meaning that we finish the spawn later, after doing a few necessary steps
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		// Setting the gameplay effect spec handle for the projectile so that the effect can be passed on when the projectile hits the target
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);
		Projectile->DamageEffectSpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		//Set damage for each damage type
		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		for(auto& Pair : DamageTypes)
		{
			const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Projectile->DamageEffectSpecHandle, Pair.Key, ScaledDamage);
		}

		// Calculate the trajectory for the projectile based on given parameters
		FVector ProjectileVelocity = FVector::ZeroVector;
		UGameplayStatics::SuggestProjectileVelocity_CustomArc(Projectile, ProjectileVelocity,SocketLocation,TargetLocation, Projectile->ProjectileMovement->GetGravityZ(), ArcModifier);
		//UE_LOG(LogTemp, Warning, TEXT("Projectile Velocity: %f"), ProjectileVelocity.Size())

		// This is a work around since sometimes the passed in TargetLocation is way further due to failed movement prediction.
		// Has only returned false when target is moving back and forth or changing movement direction in weird ways.
		if(ProjectileVelocity.Size() > ProjectileClass.GetDefaultObject()->MaxProjectileSpeed)
		{
			Projectile->Destroy();
			return false;
		}
		// Set the calculated values for the projectile
		SpawnTransform.SetRotation(ProjectileVelocity.ToOrientationQuat());
		Projectile->ProjectileMovement->InitialSpeed = ProjectileVelocity.Size();
		Projectile->ProjectileMovement->MaxSpeed = ProjectileVelocity.Size();

		// Finish the spawning process
		Projectile->FinishSpawning(SpawnTransform);
	}

	return true;
}


void UAuraProjectileSpell::SpawnProjectileWithArc_Predicted(const FGameplayTag SocketTag,
	const FVector& LaunchVelocity)
{
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
		
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddSourceObject(Projectile);
	Projectile->DamageEffectSpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
		
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	for(auto& Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Projectile->DamageEffectSpecHandle, Pair.Key, ScaledDamage);
	}
	
	SpawnTransform.SetRotation(LaunchVelocity.ToOrientationQuat());
	Projectile->ProjectileMovement->InitialSpeed = LaunchVelocity.Size();
	Projectile->ProjectileMovement->MaxSpeed = LaunchVelocity.Size();
	
	Projectile->FinishSpawning(SpawnTransform);
}
