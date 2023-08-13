// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AuraEnemy.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponentBase.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSetBase.h"
#include "AI/Controller/AuraAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "Debug/DebugFunctionLibrary.h"
#include "Game/AuraGameModeBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/CombatManager.h"
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

void AAuraEnemy::CombatManagerRegistration(ACombatManager* InCombatManager)
{
	if(HasAuthority() == false) return;
	
	CombatManager = InCombatManager;
	CombatManager->RegisterEnemy(this);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();

	if(HasAuthority()) UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);

	UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject());
	AuraUserWidget->SetWidgetController(this);
	
	const UAuraAttributeSetBase* AS = Cast<UAuraAttributeSetBase>(AttributeSet);
	if(AS)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
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
	if(bDebugHighlighted) return;
	
	bHighlighted = true;
	GetMesh()->CustomDepthStencilValue = 255;
	Weapon->CustomDepthStencilValue = 255;
	GetMesh()->SetRenderCustomDepth(true);
	
	Weapon->SetRenderCustomDepth(true);
	
	
}

void AAuraEnemy::UnHighlightActor()
{
	//UE_LOG(LogTemp, Display, TEXT("UnHighlightActor Actor called on actor: %s"), *GetName())

	//NOTE: Might not want to have a check, rather put these in an if statement.
	check(Weapon)
	check(GetMesh())

	if(bDebugHighlighted) return;
	bHighlighted = false;
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::DebugHighlightActor(TOptional<int32> OptionalStencilValue)
{
	int32 LocalStencilValue = 248;
	if(OptionalStencilValue.IsSet())
	{
		LocalStencilValue = OptionalStencilValue.GetValue();
	}
	else
	{
		LocalStencilValue = DebugStencilValue;
	}
	
	if(GetMesh() && bDebugHighlighted == false)
	{
		bDebugHighlighted = true;
		GetMesh()->CustomDepthStencilValue = LocalStencilValue;
		GetMesh()->SetRenderCustomDepth(true);
	}
	else
	{
		//Un highlight
		DebugUnHighlightActor();
	}
}

void AAuraEnemy::DebugUnHighlightActor()
{
	if(GetMesh() && bDebugHighlighted)
	{
		bDebugHighlighted = false;
		GetMesh()->CustomDepthStencilValue = 255; //RED IS DEFAULT
		GetMesh()->SetRenderCustomDepth(false);
	}
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	if(AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
	//UE_LOG(LogTemp, Warning, TEXT("HitReactChanged, Count: %d"), NewCount)
}


void AAuraEnemy::InitAbilityActorInfo()
{
	check(AbilitySystemComponent)
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponentBase>(AbilitySystemComponent)->AbilityActorInfoSet();
	
	if(HasAuthority()) InitDefaultAttributes();
}

void AAuraEnemy::InitDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(CharacterClass, Level, this, AbilitySystemComponent);
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if(HasAuthority() == false) return;
	AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	AuraAIController = Cast<AAuraAIController>(NewController);
	check(AuraAIController)
	check(BehaviorTree)
	
	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AuraAIController->RunBehaviorTree(BehaviorTree);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("Ranged"), ECharacterClass::Warrior != CharacterClass);

	AddCharacterAbilities();

	if(AuraGameMode && AuraGameMode->GetCombatManagerReady())
	{
		CombatManagerRegistration(UDebugFunctionLibrary::GetCombatManager(this));
	}
	else if (AuraGameMode && !AuraGameMode->GetCombatManagerReady())
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("CombatManagerInit"));
		GetWorldTimerManager().SetTimerForNextTick(TimerDelegate);
	}
}

void AAuraEnemy::BeginDestroy()
{
	Super::BeginDestroy();
}

void AAuraEnemy::OnDeath()
{
	if(HasAuthority())
	{
		MinionCount--;
	}
	
}

void AAuraEnemy::CombatManagerInit()
{
	
	if(AuraGameMode->GetCombatManagerReady())
	{
		CombatManagerRegistration(UDebugFunctionLibrary::GetCombatManager(this));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CombatManagerInit called too many times"))
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("CombatManagerInit"));
		GetWorldTimerManager().SetTimerForNextTick(TimerDelegate);
	}
}

void AAuraEnemy::Die()
{
	SetLifeSpan(LifeTime);
	if(AuraAIController && AuraAIController->GetBlackboardComponent()) AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	if(HasAuthority()) CombatManager->UnRegisterEnemy(this);
	Super::Die();
	
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

FVector AAuraEnemy::GetCombatSocketLocation_Implementation(const FGameplayTag& AssociatedTag)
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

EEnemyType AAuraEnemy::GetEnemyType_Implementation()
{
	return EnemyType;
}
