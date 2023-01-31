// Fill out your copyright notice in the Description page of Project Settings.


#include "Keyboards/VirtualKeyboardBaseWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/WidgetInteractionComponent.h"

#include "Buttons/KeyboardButtonBaseWidget.h"
#include "Buttons/KeyboardButtonSymbolWidget.h"
#include "Panels/VirtualKeyboardPanelBaseWidget.h"
#include "Buttons/KeyboardButtonSpecialKeyWidget.h"


void UVirtualKeyboardBaseWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime()) UpdateVisualState_Implementation(); // Happens only in the Editor
}

void UVirtualKeyboardBaseWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SubscribeAllButtonsToButtonPress();
}

void UVirtualKeyboardBaseWidget::SetupWidgetInteraction_Implementation(UWidgetInteractionComponent* WidgetInteractionComponent)
{
	WidgetInteraction = WidgetInteractionComponent;
}

void UVirtualKeyboardBaseWidget::UpdateVisualState_Implementation()
{
	FillDataArrays();
	for (auto& Button : MainButtonsArray) Button->UpdateVisuals();
}

UVirtualKeyboardPanelBaseWidget* UVirtualKeyboardBaseWidget::CreatePanel(TSubclassOf<UVirtualKeyboardPanelBaseWidget> PanelClass)
{
	if (!ensure(PanelClass)) return nullptr;
	auto PanelsParent = GetPanelsParent();
	if (!ensure(PanelsParent)) return nullptr;

	auto NewWidget = WidgetTree->ConstructWidget<UVirtualKeyboardPanelBaseWidget>(PanelClass);
	if (NewWidget)
	{
		NewWidget->SetupKeyboardPanel(this);
		PanelsParent->AddChild(NewWidget);
	}
	return NewWidget;
}

void UVirtualKeyboardBaseWidget::OnKeyboardButtonPressed(EButtonPressMethod::Type PressType, UKeyboardButtonBaseWidget* ButtonWidget)
{
	switch (ButtonWidget->GetButtonType())
	{
	case EVirtualKeyboardButtonType::UndefinedButton:
		OnButtonPressed_Custom(ButtonWidget);
		break;
	case EVirtualKeyboardButtonType::Symbol:
		if (auto SymbolButton = CastChecked<UKeyboardButtonSymbolWidget>(ButtonWidget))
			OnButtonPressed_Symbol(SymbolButton->GetSymbolAsString());
		break;
	case EVirtualKeyboardButtonType::SpecialKey:
		if (auto KeyboardKeyButton = CastChecked<UKeyboardButtonSpecialKeyWidget>(ButtonWidget))
			OnButtonPressed_KeyboardKey(PressType, KeyboardKeyButton->GetKeyboardKey());
		break;
	default:
		break;
	}
}

void UVirtualKeyboardBaseWidget::OnButtonPressed_Symbol_Implementation(const FString& Symbol)
{
	if (WidgetInteraction.IsValid()) WidgetInteraction->SendKeyChar(Symbol);
}

void UVirtualKeyboardBaseWidget::OnButtonPressed_KeyboardKey_Implementation(const EButtonPressMethod::Type PressType, const FKey& Key)
{
	if (!WidgetInteraction.IsValid()) return;
	if (PressType == EButtonPressMethod::DownAndUp) WidgetInteraction->PressAndReleaseKey(Key);
	else if (PressType == EButtonPressMethod::ButtonPress) WidgetInteraction->PressKey(Key);
	else  WidgetInteraction->ReleaseKey(Key);
}

UPanelWidget* UVirtualKeyboardBaseWidget::GetPanelsParent_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Override UVirtualKeyboardBaseWidget::GetPanelsParent in Blueprint!"));
	
	if (WidgetTree && WidgetTree->RootWidget) return Cast<UPanelWidget>(WidgetTree->RootWidget);
	return nullptr;
}

void UVirtualKeyboardBaseWidget::FillDataArrays()
{
	if (MainButtonsArray.Num() > 0 /* || AdditionalButtonsArray.Num() > 0*/ || !WidgetTree) return; // Do only once
	// TODO Should be reworked, its a lot of casts

	WidgetTree->ForEachWidget([&](UWidget* Widget) {
		if (Widget->IsA(UKeyboardButtonBaseWidget::StaticClass()))
		{
			if(auto ButtonWidget = CastChecked<UKeyboardButtonBaseWidget>(Widget))
				MainButtonsArray.Add(ButtonWidget);
		}
		/*else if (Widget->IsA(UVirtualKeyboardPanelBaseWidget::StaticClass()))
		{
			if (auto ButtonsPanelWidget = CastChecked<UVirtualKeyboardPanelBaseWidget>(Widget))
			{
				//ButtonsPanelWidget->SetupKeyboardPanel(this);
			}
		}*/
	});
}

void UVirtualKeyboardBaseWidget::SubscribeAllButtonsToButtonPress()
{
	FillDataArrays();

	for (auto& Button : MainButtonsArray)
		Button->OnKeyPressedEvent().AddUObject(this, &UVirtualKeyboardBaseWidget::OnKeyboardButtonPressed);
}
