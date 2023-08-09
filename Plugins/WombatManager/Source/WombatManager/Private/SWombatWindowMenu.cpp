// Fill out your copyright notice in the Description page of Project Settings.


#include "SWombatWindowMenu.h"
#include "SlateOptMacros.h"
#include "Characters/AuraEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Layout/SScrollBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SWombatWindowMenu::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	FSlateFontInfo TitleTextFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TitleTextFont.Size = 32;
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Wombat Manager")))
			.Font(TitleTextFont)
			.Justification(ETextJustify::Center)
			.ColorAndOpacity(FColor::White)
		]
		
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SButton)
			.Text(FText::FromString(TEXT("Get Spawned Enemies")))
			.OnClicked_Lambda([this]()
			{
				//Get all Aura Enemy actors in the level, print out the number to log,
				//Add a button to the scroll box in the wombat manager menu using the enemy's name for the button text.
				//TODO: Clicking button will highlight the enemy in the viewport, clicking again will unhighlight
				if(GEngine)
				{
					TArray<AActor*> EnemyActors;
					if(const FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport))
					{
						UGameplayStatics::GetAllActorsOfClass(WorldContext->World(), AAuraEnemy::StaticClass(), EnemyActors);
					}
				
					UE_LOG(LogTemp, Warning, TEXT("Number of Enemies: %d"), EnemyActors.Num())
					ScrollBox.Get()->ClearChildren();
					int32 i = 0;
					for(AActor* EnemyActor : EnemyActors)
					{
						++i;
						ScrollBox->AddSlot()
						.Padding(5.f)
						[
							SNew(SButton)
							.Text(FText::FromString(FString::Printf(TEXT("%d. %s"), i, *EnemyActor->GetName())))
							.OnClicked_Lambda([EnemyActor]()
							{
								if(EnemyActor->Implements<UEnemyInterface>())
								{
									Cast<IEnemyInterface>(EnemyActor)->DebugHighlightActor(253);
								}
								return FReply::Handled();
							})
						];
					}
				}
				
				return FReply::Handled();
			})
		]

		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(ScrollBox, SScrollBox)
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SSpacer)
			]
			
			//+SScrollBox::Slot()[SNew(SButton).Text(FText::FromString(TEXT("Button")))]
		]
		
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
		]
	];
	
}

FReply SWombatWindowMenu::OnTestButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Hello, world! The checkbox is %s."), (bIsTestBoxChecked ? TEXT("checked") : TEXT("unchecked")));
	return FReply::Handled();
}

void SWombatWindowMenu::OnTestCheckboxStateChanged(ECheckBoxState NewState)
{
	bIsTestBoxChecked = NewState == ECheckBoxState::Checked ? true : false;
}

ECheckBoxState SWombatWindowMenu::IsTestBoxChecked() const
{
	return bIsTestBoxChecked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
