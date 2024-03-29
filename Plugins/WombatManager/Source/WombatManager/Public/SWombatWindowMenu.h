// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SScrollBox.h"

class ACombatManager;
/**
 * 
 */
class WOMBATMANAGER_API SWombatWindowMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SWombatWindowMenu)
	{}
	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetsDataArray)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual ~SWombatWindowMenu() override;

	FReply OnTestButtonClicked();
	void OnTestCheckboxStateChanged(ECheckBoxState NewState);
	ECheckBoxState IsTestBoxChecked() const;

	void OnLevelChanged(const bool bYes);
	FDelegateHandle OnLevelChangedDelegateHandle;

	void OnEditorModeChanged(FEditorModeID  NewEditorMode);

protected:
	bool bIsTestBoxChecked = false;

	TSharedPtr<SScrollBox> ScrollBox;

	TSharedPtr<SButton> LastButtonPressed;

	TArray<TSharedPtr<SButton>> EnemyButtons;
	
	TArray<TSharedPtr<SButton>> SquadButtons;

	void OnColorButtonCommitted(FLinearColor InColor);

private:

	TArray<AActor*> EnemyActors;
	TObjectPtr<ACombatManager> CombatManager;
};
