// Fill out your copyright notice in the Description page of Project Settings.

#include "Buttons/KeyboardButtonBaseWidget.h"

#include "Components/Button.h"

void UKeyboardButtonBaseWidget::UpdateVisuals()
{
	if (bUseCustomStyle) Button_Main->SetStyle(ButtonStyle);
}