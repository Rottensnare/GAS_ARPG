// Fill out your copyright notice in the Description page of Project Settings.


#include "SWombatWindowMenu.h"
#include "SlateOptMacros.h"
#include "Brushes/SlateBoxBrush.h"
#include "Characters/AuraEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Colors/SColorPicker.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SWombatWindowMenu::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;
	//EnemyButtons.SetNumUninitialized(50); //Do something else. Hard coding almost always bad.
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
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
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
						for(AActor* Enemy : EnemyActors)
						{
							Cast<IEnemyInterface>(Enemy)->DebugUnHighlightActor();
						}
						EnemyActors.Empty();
						
						if(const FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport))
						{
							UGameplayStatics::GetAllActorsOfClass(WorldContext->World(), AAuraEnemy::StaticClass(), EnemyActors);
						}
						
						UE_LOG(LogTemp, Warning, TEXT("Number of Enemies: %d"), EnemyActors.Num())
						ScrollBox.Get()->ClearChildren();
						EnemyButtons.Empty();
						int32 i = 0;
						for(AActor* EnemyActor : EnemyActors)
						{
							++i;
							TSharedPtr<SButton> NewButton;
							ScrollBox->AddSlot()
							.Padding(5.f)
							[
								//SAssignNew( EnemyButtons[i], SButton)
								 SAssignNew(NewButton, SButton)
								.Text(FText::FromString(FString::Printf(TEXT("%d. %s"), i, *EnemyActor->GetName())))
								.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("Docking.SidebarButton.Opened"))
								.OnClicked_Lambda([this, EnemyActor, NewButton, i]()
								{
									if(EnemyButtons[i-1].Get()->GetTag() != FName("Used"))
									{
										EnemyButtons[i-1]->SetButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("PrimaryButton"));
										EnemyButtons[i-1].Get()->SetTag(FName("Used"));
									}
									else
									{
										EnemyButtons[i-1]->SetButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("Docking.SidebarButton.Opened"));
										EnemyButtons[i-1].Get()->SetTag(NAME_None);
									}
									
									if(EnemyActor->Implements<UEnemyInterface>())
									{
										Cast<IEnemyInterface>(EnemyActor)->DebugHighlightActor(252);
									}
									return FReply::Handled();
								})
								
							];
							EnemyButtons.Add(NewButton);
						}
					}
						
					return FReply::Handled();
				})
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
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
			//+SHorizontalBox::Slot()
			//.AutoWidth()
			//[
				/*SAssignNew(LastButtonPressed, SButton)
				.Text(FText::FromString(TEXT("Color Picker")))
				.OnClicked_Lambda([this]()
				{
					FColorPickerArgs PickerArgs;
					PickerArgs.bOnlyRefreshOnMouseUp = true;
					PickerArgs.ParentWidget = AsShared();
					
					PickerArgs.bUseAlpha = false;
					PickerArgs.bOnlyRefreshOnOk = false;
					PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &SWombatWindowMenu::OnColorButtonCommitted);
					
					OpenColorPicker(PickerArgs);
					return FReply::Handled();
				})
				*/
			//]
			
			//+SScrollBox::Slot()[SNew(SButton).Text(FText::FromString(TEXT("Button")))]
		]
		
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
		]
	];
	
}

SWombatWindowMenu::~SWombatWindowMenu()
{
	for(AActor* Enemy : EnemyActors)
	{
		//Shouldn't fail ever
		Cast<IEnemyInterface>(Enemy)->DebugUnHighlightActor();
	}
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

void SWombatWindowMenu::OnColorButtonCommitted(FLinearColor InColor)
{
	//BUG: Something about this crashes the editor but I have no idea what.
	//BUG: For whatever reason I don't have symbols for slatecore, so I can't really debug it easily.
	//BUG: Low on space and symbols can take dozens of GB of space, so maybe not worth trying to load.
	/*
	FButtonStyle NewButtonStyle = FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button");
	
	FSlateBrush SlateBrush;
	SlateBrush.TintColor = FSlateColor(InColor);
	NewButtonStyle.SetNormal(SlateBrush);

	if(LastButtonPressed.IsValid()) LastButtonPressed->SetButtonStyle(&NewButtonStyle);
	*/
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
