// Fill out your copyright notice in the Description page of Project Settings.


#include "Debug/DebugWidget.h"

void UDebugWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
