// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/GameplayAbility/AuraProjectileSpell.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 666.f;
	ProjectileMovement->MaxSpeed = 666.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;

}

// Called when the game starts or when spawned
void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();

	LoopSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
	SetLifeSpan(ProjectileLifeSpan);
}

void AAuraProjectile::Destroyed()
{
	if(!bHit && !HasAuthority())
	{
		if(ImpactSound && ImpactSoundAttenuation) UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator, VolumeMultiplier, 1.f, 0.f, ImpactSoundAttenuation);
		if(ImpactEffect) UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	}

	if(LoopSoundComponent)
	{
		LoopSoundComponent->Stop();
		LoopSoundComponent->DestroyComponent();
	} 
	
	Super::Destroyed();
}

void AAuraProjectile::OnProjectileOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(DamageEffectSpecHandle.Data.Get() == nullptr || DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser() == OtherActor) return;
	if(OtherActor == DamageEffectSpecHandle.Data.Get()->GetContext().GetInstigator() || OtherActor == this) return;
	if(ImpactSound && ImpactSoundAttenuation) UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator, VolumeMultiplier, 1.f, 0.f, ImpactSoundAttenuation);
	if(ImpactEffect) UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	
	
	if(HasAuthority())
	{
		if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const UAuraProjectileSpell* ProjectileSpell = Cast<UAuraProjectileSpell>(DamageEffectSpecHandle.Data.Get()->GetContext().GetAbility());
			const bool bIsFriend = UAuraAbilitySystemLibrary::IsFriend(OtherActor, DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser());
			if(ProjectileSpell)
			{
				if(!ProjectileSpell->GetFriendlyFire() && !bIsFriend || ProjectileSpell->GetFriendlyFire())
				{
					TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
				}
			}
			else
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
			}
		}
		
		Destroy();
	}
	else
	{
		bHit = true;
	}
}
