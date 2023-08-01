// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponentBase.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bDoCollisionTest = false;
	
	MainCamera = CreateDefaultSubobject<UCameraComponent>("MainCamera");
	MainCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	MainCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	LevelUpEffect = CreateDefaultSubobject<UNiagaraComponent>("LevelUpEffectComponent");
	LevelUpEffect->SetupAttachment(GetRootComponent());
	LevelUpEffect->bAutoActivate = false;
	
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponentBase>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();

	if(IsLocallyControlled())
	{
		AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController());
		if(AuraPlayerController)
		{
			AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD());
			if(AuraHUD) AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
			else UE_LOG(LogTemp, Error, TEXT("AuraHUD was nullptr in InitAbilityActorInfo of %s"), *GetName())
		}
		else UE_LOG(LogTemp, Error, TEXT("AuraPlayerController was nullptr in InitAbilityActorInfo of %s"), *GetName())
	}

	InitDefaultAttributes();
	
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	AddCharacterAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

void AAuraCharacter::MulticastLevelUpParticles_Implementation()
{
	if(IsValid(LevelUpEffect))
	{
		LevelUpEffect->SetWorldRotation(FRotator::MakeFromEuler(FVector(0.f, 0.f, 180.f)));
		LevelUpEffect->Activate(true);
	}
	if(IsValid(LevelUpSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, LevelUpSound, GetActorLocation(), FRotator::ZeroRotator);
	}
}

int32 AAuraCharacter::GetCharacterLevel_Implementation()
{
	const AAuraPlayerState* const AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	return AuraPlayerState->GetCharacterLevel();
}

int32 AAuraCharacter::GetXP_Implementation() const
{
	const AAuraPlayerState* const AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	return AuraPlayerState->GetXP();
}

int32 AAuraCharacter::FindLevelForXP_Implementation(const int32 InXP)
{
	const AAuraPlayerState* const AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	return AuraPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

FVector AAuraCharacter::GetCombatSocketLocation_Implementation(const FGameplayTag& AssociatedTag)
{
	if(!TagsToSockets.Contains(AssociatedTag))
	{
		UE_LOG(LogTemp, Error, TEXT("AAuraEnemy::GetCombatSocketLocation_Implementation: TagsToSockets does not contain Tag: [%s]"), *AssociatedTag.GetTagName().ToString())
		return FVector::ZeroVector;
	}
	if(AssociatedTag.MatchesTagExact(FAuraGameplayTags::Get().CombatSocket_Weapon))
	{
		return Weapon->GetSocketLocation(TagsToSockets[AssociatedTag]);
	}

	return GetMesh()->GetSocketLocation(TagsToSockets[AssociatedTag]);
}

FFramePackage AAuraCharacter::GetFramePackage_Implementation(const int32 Index)
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

void AAuraCharacter::AddToXP_Implementation(const int32 InXP)
{
	AAuraPlayerState* const AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	AuraPlayerState->AddToXP(InXP);
}

void AAuraCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

int32 AAuraCharacter::GetAttributePointsReward_Implementation(const int32 InLevel) const
{
	const AAuraPlayerState* const AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	if(AuraPlayerState->LevelUpInfo->LevelUpInformation.Num() < InLevel) return 0;
	return AuraPlayerState->LevelUpInfo->LevelUpInformation[InLevel].AttributePointReward;
}

int32 AAuraCharacter::GetSpellPointsReward_Implementation(const int32 InLevel) const
{
	const AAuraPlayerState* const AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	if(AuraPlayerState->LevelUpInfo->LevelUpInformation.Num() < InLevel) return 0;
	return AuraPlayerState->LevelUpInfo->LevelUpInformation[InLevel].SpellPointReward;
}

void AAuraCharacter::AddToPlayerLevel_Implementation(const int32 InLevel)
{
	AAuraPlayerState* const AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    check(AuraPlayerState)
	AuraPlayerState->AddToLevel(InLevel);
}

void AAuraCharacter::AddToAttributePoints_Implementation(const int32 InAttributePoints)
{
	AAuraPlayerState* const AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)
	
}

void AAuraCharacter::AddToSpellPoints_Implementation(const int32 InSpellPoints)
{
	AAuraPlayerState* const AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)
}

