// Fill out your copyright notice in the Description page of Project Settings.


#include "Panels/VirtualKeyboardPanelBaseWidget.h"

#include "Blueprint/WidgetTree.h"

#include "Buttons/KeyboardButtonBaseWidget.h"
#include "Keyboards/VirtualKeyboardBaseWidget.h"

void UVirtualKeyboardPanelBaseWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (WidgetTree)
	{
		WidgetTree->ForEachWidget([&](UWidget* Widget) {
			if (Widget->IsA(UKeyboardButtonBaseWidget::StaticClass()))
			{
				if (auto ButtonWidget = CastChecked<UKeyboardButtonBaseWidget>(Widget))
					ButtonsArray.Add(ButtonWidget);
			}
		});
	}
}

void UVirtualKeyboardPanelBaseWidget::SetupKeyboardPanel(UVirtualKeyboardBaseWidget* OwningKeyboard)
{
	KeyboardParent = OwningKeyboard;

	for (auto& Button : ButtonsArray)
		Button->OnKeyPressedEvent().AddUObject(KeyboardParent, &UVirtualKeyboardBaseWidget::OnKeyboardButtonPressed);
}