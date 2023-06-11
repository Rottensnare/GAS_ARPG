// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponentBase.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interfaces/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController() :
ControlledPawn(nullptr)
{
	bReplicates = true;
	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	
}

void AAuraPlayerController::AutoRun()
{
	if(bAutoRunning && ControlledPawn)
	{
		const FVector LocationOnSpline = SplineComponent->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = SplineComponent->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Size();
		if(DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	CursorTrace();
	AutoRun();

	Super::PlayerTick(DeltaTime);
}

void AAuraPlayerController::ShowDamageNumber_Implementation(const float Damage, AActor* TargetActor, const bool bBlockedHit, const bool bCriticalHit)
{
	if(IsValid(TargetActor) && DamageTextCompClass)
	{
		UDamageTextComponent* DmgTextComp = NewObject<UDamageTextComponent>(TargetActor, DamageTextCompClass);
		if(DmgTextComp != nullptr)
		{
			DmgTextComp->RegisterComponent();
			DmgTextComp->AttachToComponent(TargetActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			DmgTextComp->SetRelativeLocation(DmgTextComp->GetRelativeLocation() + FloatingTextOffset);
			DmgTextComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			DmgTextComp->SetDamageText(Damage, bBlockedHit, bCriticalHit);
		}
	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	checkf(AuraContext, TEXT("AuraContext wasn't set."))

	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	
	if(IsLocalController())
	{
		checkf(Subsystem, TEXT("EnhancedInputLocalPlayerSubsystem was nullptr"))
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeGameAndUI;
	InputModeGameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeGameAndUI.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeGameAndUI);

	ControlledPawn = GetPawn<APawn>();
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* EnhancedInputComponent = CastChecked<UAuraInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
	EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &ThisClass::ShiftPressed);
	EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &ThisClass::ShiftReleased);
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//UE_LOG(LogTemp, Warning, TEXT("Pressed, Tag: %s"), *InputTag.ToString())
	if(InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = CurrentActor ? true : false;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if(GetASC() == nullptr) return;
	if(InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB) == false)
	{
		GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}
	
	GetASC()->AbilityInputTagReleased(InputTag);

	if(!bTargeting && !bShiftKeyDown)
	{
		if(FollowTime < ShortPressThreshold)
		{
			UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, GetPawn()->GetActorLocation(), CachedDestination);
			if(NavPath)
			{
				SplineComponent->ClearSplinePoints();
				for(const FVector& Point : NavPath->PathPoints)
				{
					SplineComponent->AddSplinePoint(Point, ESplineCoordinateSpace::World);
					//DrawDebugBox(GetWorld(), Point, FVector(5.f), FColor::Blue, false, 5.f);
				}
				bAutoRunning = true;
				if(NavPath->PathPoints.IsEmpty() == false) CachedDestination = NavPath->PathPoints.Last();
			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if(GetASC() == nullptr) return;
	
	if(InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if(bTargeting || bShiftKeyDown)
		{
			GetASC()->AbilityInputTagHeld(InputTag);
			return;
		}
		FollowTime += GetWorld()->GetDeltaSeconds();

		if(CursorHitResult.bBlockingHit)
		{
			CachedDestination = CursorHitResult.ImpactPoint;
		}
		if(APawn* CurrentPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - CurrentPawn->GetActorLocation()).GetSafeNormal();
			CurrentPawn->AddMovementInput(WorldDirection, 1.f, false);
		}
	}
	else
	{
		GetASC()->AbilityInputTagHeld(InputTag);
	}
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	ControlledPawn = ControlledPawn == nullptr ? GetPawn<APawn>() : ControlledPawn;
	if(ControlledPawn)
	{
		ControlledPawn->AddMovementInput(ForwardDirection * InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection * InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHitResult);
	if(CursorHitResult.bBlockingHit == false) return;

	LastActor = CurrentActor;
	CurrentActor = Cast<IEnemyInterface>(CursorHitResult.GetActor());

	//	Not hovering over highlightable actors
	if(LastActor == nullptr && CurrentActor == nullptr) return; 

	// If no actor was previously highlighted but currently hovered actor is highlightable
	if(LastActor == nullptr && CurrentActor != nullptr)
	{
		CurrentActor->HighlightActor();
		return;
	}

	// If current actor is not highlightable but previous was.
	if(LastActor != nullptr && CurrentActor == nullptr)
	{
		LastActor->UnHighlightActor();
		return;
	}

	// If both actors are highlightable
	if(LastActor != nullptr && CurrentActor != nullptr)
	{
		// If actors are not equal
		if(LastActor != CurrentActor)
		{
			LastActor->UnHighlightActor();
			CurrentActor->HighlightActor();
		}

		// If same, do nothing
	}
}

UAuraAbilitySystemComponentBase* AAuraPlayerController::GetASC()
{
	if(AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponentBase>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
	
		
}
