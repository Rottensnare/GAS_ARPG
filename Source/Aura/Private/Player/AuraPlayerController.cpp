// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interfaces/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController() :
ControlledPawn(nullptr)
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	CursorTrace();

	Super::PlayerTick(DeltaTime);
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

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
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
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	if(HitResult.bBlockingHit == false) return;

	LastActor = CurrentActor;
	CurrentActor = Cast<IEnemyInterface>(HitResult.GetActor());

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
