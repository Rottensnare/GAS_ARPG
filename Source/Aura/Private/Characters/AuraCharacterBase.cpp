// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AuraCharacterBase.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponentBase.h"
#include "AbilitySystem/AuraAttributeSetBase.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "Debug/DebugFunctionLibrary.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	
}

void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraCharacterBase, MinionCount);
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	StartCustomTick();
}

void AAuraCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(FrameHistory.Num() < 2) return;
	LeanDotProduct = FVector::DotProduct(
		FVector::CrossProduct(GetActorForwardVector(), FVector::UpVector),
		FrameHistory.GetHead()->GetNextNode()->GetValue().ActorForwardVector);
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
	
}

void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()))
	check(GameplayEffectClass)

	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle GESpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*GESpecHandle.Data.Get(), AbilitySystemComponent);
}


void AAuraCharacterBase::InitDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void AAuraCharacterBase::AddCharacterAbilities()
{
	if(!HasAuthority()) return;
	
	UAuraAbilitySystemComponentBase* AuraASC = CastChecked<UAuraAbilitySystemComponentBase>(AbilitySystemComponent);
	AuraASC->AddCharacterAbilities(DefaultAbilities);
}

void AAuraCharacterBase::Dissolve()
{
	if(IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMaterialInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMaterialInst);
		StartDissolveTimeline(DynamicMaterialInst);
	}
	
	if(IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMaterialInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMaterialInst);
		StartWeaponDissolveTimeline(DynamicMaterialInst);
	}
	
}

void AAuraCharacterBase::OnRep_MinionCount()
{
	
}

void AAuraCharacterBase::SaveFramePackage()
{
	if(bSaveFrameHistory == false) return;
	if(HasAuthority() == false) return;

	if(FrameHistory.Num() <= 1)
	{
		FFramePackage CurrentFrame;
		SaveFramePackage(CurrentFrame);
		FrameHistory.AddHead(CurrentFrame);
	}
	else
	{
		float HistoryLenght = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;

		while(HistoryLenght > MaxRecordTime)
		{
			FrameHistory.RemoveNode(FrameHistory.GetTail());
			HistoryLenght = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		}
		
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
	}
}

void AAuraCharacterBase::SaveFramePackage(FFramePackage& OutPackage) const
{
	if(bSaveFrameHistory == false) return;
	OutPackage.Time = GetWorld()->GetTimeSeconds();
	OutPackage.ActorLocation = GetActorLocation();
	OutPackage.ActorVelocity = GetVelocity();
	OutPackage.ActorForwardVector = GetActorForwardVector();
}

void AAuraCharacterBase::VisualizeFrameHistory()
{
	for(const auto& Frame : FrameHistory)
	{
		UKismetSystemLibrary::DrawDebugBox(
			this,
			Frame.ActorLocation,
			FVector(10.f),
			FLinearColor::Blue);

		UKismetSystemLibrary::DrawDebugArrow(
			this,
			Frame.ActorLocation,
			Frame.ActorLocation + Frame.ActorVelocity,
			2.f,
			FLinearColor::Green);
	}
}

void AAuraCharacterBase::ExtrapolateFrameHistory(const float ExtrapolationTime)
{
	
}

void AAuraCharacterBase::CustomTick()
{
	SaveFramePackage();

	//StartCustomTick();
}

void AAuraCharacterBase::StartCustomTick()
{
	const float TickDelay = 1.f / CustomTickRate;
	GetWorldTimerManager().SetTimer(CustomTickHandle, this, &ThisClass::CustomTick, TickDelay, true);
}

void AAuraCharacterBase::StopCustomTick()
{
	GetWorldTimerManager().ClearTimer(CustomTickHandle);
}

bool AAuraCharacterBase::RunningInCircles(const float Threshold, const FVector& CircleCenter)
{
	if(FrameHistory.Num() < 5) return false;

	/*FVector Centroid = FVector::ZeroVector;
	for (const auto& Frame : FrameHistory)
	{
		Centroid += Frame.ActorLocation;
	}
	Centroid /= FrameHistory.Num();*/
	
	LeanDotProduct = FVector::DotProduct(FVector::CrossProduct(GetActorForwardVector(), FVector::UpVector), FrameHistory.GetHead()->GetNextNode()->GetValue().ActorForwardVector);
	/*
	const FVector CrossPoint = UDebugFunctionLibrary::GetVectorIntersectionPoint(
		GetActorLocation(),
		GetActorLocation() + GetVelocity() * 1000.f,
		FrameHistory.GetHead()->GetNextNode()->GetValue().ActorLocation,
		FrameHistory.GetHead()->GetNextNode()->GetValue().ActorVelocity * 1000.f);

	UKismetSystemLibrary::DrawDebugSphere(
			this,
			CrossPoint,
			50.f,
			8,
			FLinearColor::White);
	
	UKismetSystemLibrary::DrawDebugSphere(
			this,
			Centroid,
			25.f,
			8,
			FLinearColor::Black);
	
	float TotalDistance = 0.0f;
	*/

	
	TArray<float> DistancesToCenter;
	for (const auto& Frame : FrameHistory)
	{
		float Distance = FVector::Distance(Frame.ActorLocation, CircleCenter);
		DistancesToCenter.Add(Distance);
	}
	
	const float StandardDeviation = UDebugFunctionLibrary::GetStandardDeviation(DistancesToCenter);
	//UE_LOG(LogTemp, Display, TEXT("AuraCharacterBase: Standard Deviation: %f"), StandardDeviation)

	bRunningInCircles = StandardDeviation <= Threshold;
	return bRunningInCircles;
}


UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

UAnimMontage* AAuraCharacterBase::GetAttackMontage_Implementation()
{
	return AttackMontage;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

void AAuraCharacterBase::Die()
{
	AbilitySystemComponent->AddLooseGameplayTag(FAuraGameplayTags::Get().Status_Dead);
	if(Weapon) Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath();
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{ 
	return this;
}

bool AAuraCharacterBase::IsRunningInCircles_Implementation(const float Threshold, const FVector& CircleCenter)
{
	if(FrameHistory.Num() < 5) return false;
	if(GetVelocity().Size() < 5.f) return false; // False if the character is basically staying still.
	if(FMath::Abs(LeanDotProduct) < 0.07f) return false; // False if character is turning at a very small rate. 
	TArray<float> DistancesToCenter;
	for (const auto& Frame : FrameHistory)
	{
		float Distance = FVector::Distance(Frame.ActorLocation, CircleCenter);
		DistancesToCenter.Add(Distance);
	}
	
	const float StandardDeviation = UDebugFunctionLibrary::GetStandardDeviation(DistancesToCenter);
	//UE_LOG(LogTemp, Display, TEXT("AuraCharacterBase: Standard Deviation: %f"), StandardDeviation)

	bRunningInCircles = StandardDeviation <= Threshold;
	return bRunningInCircles;
}

FFramePackage AAuraCharacterBase::GetFramePackage_Implementation(const int32 Index)
{
	if(FrameHistory.Num() <= Index) return FFramePackage();
	int32 I = 0;
	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Node = FrameHistory.GetHead();
	while(I < Index)
	{
		Node = Node->GetNextNode();
		if(Node == nullptr) return FFramePackage();
		I++;
	}

	return Node->GetValue();
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
	if(!HasAuthority())
	{
		if(AbilitySystemComponent) AbilitySystemComponent->AddLooseGameplayTag(FAuraGameplayTags::Get().Status_Dead);
	}

	if(Weapon)
	{
		Weapon->SetSimulatePhysics(true);
		Weapon->SetEnableGravity(true);
		Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	
	if(GetMesh())
	{
		GetMesh()->SetEnableGravity(true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	}
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Dissolve();

	if(DeathSound && BaseSoundAttenuation)
	{
		UGameplayStatics::PlaySoundAtLocation(
		this,
		DeathSound,
		GetActorLocation(),
		FRotator::ZeroRotator,
		0.25f,
		1.f,
		0.f,
		BaseSoundAttenuation);
	}
	

	bDead = true;
}


