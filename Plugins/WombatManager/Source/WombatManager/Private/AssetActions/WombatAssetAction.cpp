// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/WombatAssetAction.h"

#include "EditorAssetLibrary.h"
#include "EditorUtilityLibrary.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

void UWombatAssetAction::Debug()
{
	TArray<FAssetData> SelectedAssetData = UEditorUtilityLibrary::GetSelectedAssetData();
	for(const FAssetData& AssetData : SelectedAssetData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Asset Name: %s"), *AssetData.AssetName.ToString())
		FText DialogueTitle = FText::FromString(TEXT("Some Title"));
		const EAppReturnType::Type ReturnMsg = FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Debug Message")), &DialogueTitle);
		if(ReturnMsg == EAppReturnType::Type::Ok)
		{
			FNotificationInfo NotificationInfo = FNotificationInfo(FText::FromString(TEXT("Oki Doki")));
			NotificationInfo.bUseLargeFont = true;
			NotificationInfo.FadeOutDuration = 4.f;
			FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		}
	}
}
