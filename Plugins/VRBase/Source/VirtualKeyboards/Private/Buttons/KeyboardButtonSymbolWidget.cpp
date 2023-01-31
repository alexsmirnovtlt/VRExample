// Fill out your copyright notice in the Description page of Project Settings.


#include "Buttons/KeyboardButtonSymbolWidget.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"

UKeyboardButtonSymbolWidget::UKeyboardButtonSymbolWidget(const FObjectInitializer& ObjectInitializer)
{
	ButtonType = EVirtualKeyboardButtonType::Symbol;
}

void UKeyboardButtonSymbolWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	bIsInRegularState = bRegularIsDefault;
	if (IsDesignTime()) UpdateVisuals(); // Updates buttons in widget editor. Buttons react to certain changes like text and style
}

void UKeyboardButtonSymbolWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	bIsInRegularState = bRegularIsDefault;
	UpdateVisuals();
	Button_Main->OnReleased.AddDynamic(this, &UKeyboardButtonSymbolWidget::OnButtonReleased);
}

void UKeyboardButtonSymbolWidget::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

	bIsInRegularState = bRegularIsDefault;
	// TODO Same line appears 3 times because NativePreConstruct and NativeOnInitialized wont get called when we open level in the editor and OnWidgetRebuilt wont get called at other times
	// Need to only do it once somewhere

	UpdateVisuals();
}

void UKeyboardButtonSymbolWidget::UpdateVisuals()
{
	Super::UpdateVisuals();

	FText& NewText = bIsInRegularState ? RegularSymbol : CapitalSymbol;
	ChangeText(NewText);
}

void UKeyboardButtonSymbolWidget::SwitchState(bool bRegular)
{
	if (!bCanSwitchBetweenCapitalAndRegular) return;
	bIsInRegularState = bRegular;
	UpdateVisuals();
}

void UKeyboardButtonSymbolWidget::ToggleState(bool& IsRegular)
{
	bIsInRegularState = !bIsInRegularState;
	SwitchState(bIsInRegularState);
	IsRegular = bIsInRegularState;
}

const FString& UKeyboardButtonSymbolWidget::GetSymbolAsString() const
{
	return bIsInRegularState ? RegularSymbol.ToString() : CapitalSymbol.ToString();
}

void UKeyboardButtonSymbolWidget::ChangeText(const FText& NewText)
{
	if(TextBlock_Symbol) TextBlock_Symbol->SetText(NewText);
}

void UKeyboardButtonSymbolWidget::OnButtonReleased()
{
	BroadcastPressEvent(EButtonPressMethod::DownAndUp);
}