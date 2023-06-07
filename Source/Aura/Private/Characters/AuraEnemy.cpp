// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AuraEnemy.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponentBase.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSetBase.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/AuraUserWidget.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponentBase>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSetBase>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();

	UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent);

	UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject());
	AuraUserWidget->SetWidgetController(this);
	
	const UAuraAttributeSetBase* AS = Cast<UAuraAttributeSetBase>(AttributeSet);
	if(AS)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddLambda(
			[this, AS](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			});

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).AddLambda(
			[this, AS](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			});

		AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HitReactTagChanged);

		OnHealthChanged.Broadcast(AS->GetHealth());
		OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
	}
	
}

void AAuraEnemy::HighlightActor()
{
	//UE_LOG(LogTemp, Display, TEXT("HighlightActor Actor called on actor: %s"), *GetName())

	//NOTE: Might not want to have a check, rather put these in an if statement.
	check(Weapon)
	check(GetMesh())
	
	bHighlighted = true;
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
}

void AAuraEnemy::UnHighlightActor()
{
	//UE_LOG(LogTemp, Display, TEXT("UnHighlightActor Actor called on actor: %s"), *GetName())

	//NOTE: Might not want to have a check, rather put these in an if statement.
	check(Weapon)
	check(GetMesh())
	
	bHighlighted = false;
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	UE_LOG(LogTemp, Warning, TEXT("HitReactChanged, Count: %d"), NewCount)
}


void AAuraEnemy::InitAbilityActorInfo()
{
	check(AbilitySystemComponent)
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponentBase>(AbilitySystemComponent)->AbilityActorInfoSet();
	
	InitDefaultAttributes();
}

void AAuraEnemy::InitDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(CharacterClass, Level, this, AbilitySystemComponent);
}

void AAuraEnemy::Die()
{
	SetLifeSpan(LifeTime);
	
	Super::Die();
}
