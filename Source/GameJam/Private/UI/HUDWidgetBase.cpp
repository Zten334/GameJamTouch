// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HUDWidgetBase.h"
#include "Components/PlayerUIComponent.h"

void UHUDWidgetBase::Init(UPlayerUIComponent* InUIComponent)
{
	UIComponent = InUIComponent;
}
