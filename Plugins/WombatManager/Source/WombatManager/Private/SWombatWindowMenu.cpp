// Fill out your copyright notice in the Description page of Project Settings.


#include "SWombatWindowMenu.h"

#include "EditorUtilitySubsystem.h"
#include "SlateOptMacros.h"
#include "Brushes/SlateBoxBrush.h"
#include "Characters/AuraEnemy.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Colors/SColorPicker.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

//BUG: WILL CRASH THE EDITOR IF YOU SWITCH BETWEEN LEVELS AND PRESS THE BUTTONS. NEED TO CLOSE AND REOPEN WHEN SWITCHING BETWEEN LEVELS!
void SWombatWindowMenu::Construct(const FArguments& InArgs)
{
	//TODO: Remove Lambdas and use normal delegates.
	//TODO: Maybe rework FCombatant and FSquad to classes.
	FEditorDelegates::PostPIEStarted.AddSP(this, &SWombatWindowMenu::OnLevelChanged);
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
					//Highlight the enemy of the corresponding button.
					if(GEngine)
					{
						for(AActor* Enemy : EnemyActors)
						{
							if(IsValid(Enemy) && Enemy->Implements<UEnemyInterface>())
							{
								Cast<IEnemyInterface>(Enemy)->DebugUnHighlightActor();
							}
						}
						EnemyActors.Empty();

						//Get all enemies in the current level. Current you need to press the button every time you switch between PIE and Editor.
						//Currently the list of enemies is not updated automatically when an enemy is spawned or destroyed.
						if(const FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport))
						{
							UGameplayStatics::GetAllActorsOfClass(WorldContext->World(), AAuraEnemy::StaticClass(), EnemyActors);
						}
						
						//UE_LOG(LogTemp, Warning, TEXT("Number of Enemies: %d"), EnemyActors.Num())
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
								 SAssignNew(NewButton, SButton)
								.Text(FText::FromString(FString::Printf(TEXT("%d. %s"), i, *EnemyActor->GetName())))
								.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("Docking.SidebarButton.Opened"))
								.OnClicked_Lambda([this, EnemyActor, NewButton, i]() //Lambda inside a Lambda? We need to go deeper
								{
									//When button has NOT been pressed = Enemy is NOT being highlighted
									if(EnemyButtons[i-1].Get()->GetTag() != FName("Used"))
									{
										//Change the button style to indicate that the enemy it represents is now being highlighted and mark the button as "Used"
										EnemyButtons[i-1]->SetButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("PrimaryButton"));
										EnemyButtons[i-1].Get()->SetTag(FName("Used"));
									}
									else //If button has been pressed = Enemy is currently being highlighted
									{
										//Change the button style to indicate that the enemy it represents is not being highlighted and mark the button as not used
										EnemyButtons[i-1]->SetButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("Docking.SidebarButton.Opened"));
										EnemyButtons[i-1].Get()->SetTag(NAME_None);
									}
									
									if(EnemyActor->Implements<UEnemyInterface>())
									{
										//DebugHighlightActor takes the highlight color, checks if enemy is being currently highlighted,
										//if not, highlight the enemy, if highlighted then call DebugUnHighlightActor
										//See PP_Highlight for what value corresponds to what color.
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
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("Get Squads")))
				.OnClicked_Lambda([this]()
				{
					ScrollBox.Get()->ClearChildren();
					SquadButtons.Empty();
					//CombatManager = nullptr;
					
					if(const FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport))
					{
						if(!WorldContext->World()->IsGameWorld())
						{
							return FReply::Handled();
						}
						
						if(AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext->World())))
						{
							if(!IsValid(CombatManager))
							{
								CombatManager = AuraGameMode->GetCombatManager();
							}
						}
					}
					if(CombatManager.Get() == nullptr) return FReply::Handled();

					
					int32 i = 0;
					if(ScrollBox == nullptr || CombatManager->Squads.IsEmpty()) return FReply::Handled();
					for(const FSquad Squad : CombatManager->Squads)
					{
						++i;
						TSharedPtr<SButton> NewButton;
						ScrollBox->AddSlot()
						.Padding(5.f)
						[
							SAssignNew(NewButton, SButton)
							.Text(FText::FromString(FString::Printf(TEXT("Squad %d"), Squad.SquadID)))
							.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("Docking.SidebarButton.Opened"))
							.OnClicked_Lambda([Squad]()
							{
								if(const FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport))
								{
									if(!WorldContext->World()->IsGameWorld()) return FReply::Handled();
								}
								
								for(const FCombatant& Combatant : Squad.Members)
								{
									if(!FCombatant::IsNullCombatant(Combatant))
									{
										Combatant.Enemy->DebugHighlightActor();
									}
								}
									
								return FReply::Handled();
							})
						];
						SquadButtons.Add(NewButton);
					}
					
					return FReply::Handled();
				})
			]
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
	ScrollBox.Get()->ClearChildren();
	EnemyButtons.Empty();
	SquadButtons.Empty();
	CombatManager = nullptr;
	
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

void SWombatWindowMenu::OnLevelChanged(const bool bYes)
{
	//const FArguments Args;
	//Construct(Args);
	UE_LOG(LogTemp, Warning, TEXT("PIE Started: %d"), bYes)
	
}

void SWombatWindowMenu::OnEditorModeChanged(FEditorModeID NewEditorMode)
{
	UE_LOG(LogTemp, Warning, TEXT("New Editor Mode: %s"), *NewEditorMode.ToString());
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
