// Fill out your copyright notice in the Description page of Project Settings.


#include "Buttons/KeyboardButtonSpecialKeyWidget.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"

UKeyboardButtonSpecialKeyWidget::UKeyboardButtonSpecialKeyWidget(const FObjectInitializer& ObjectInitializer)
{
	ButtonType = EVirtualKeyboardButtonType::SpecialKey;
}

void UKeyboardButtonSpecialKeyWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime()) UpdateVisuals();
}

void UKeyboardButtonSpecialKeyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Main->OnPressed.AddDynamic(this, &UKeyboardButtonSpecialKeyWidget::OnButtonPressed);
	Button_Main->OnReleased.AddDynamic(this, &UKeyboardButtonSpecialKeyWidget::OnButtonReleased);

	UpdateVisuals();
}

void UKeyboardButtonSpecialKeyWidget::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

	UpdateVisuals();
}

void UKeyboardButtonSpecialKeyWidget::UpdateVisuals()
{
	Super::UpdateVisuals();

	if (TextBlock_Main)
	{
		TextBlock_Main->SetText(Text);
		ESlateVisibility TextVisibility = Text.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::Visible;
		TextBlock_Main->SetVisibility(TextVisibility);
	}

	if (Image_Main)
	{
		if (Image.GetResourceName().IsNone())
		{
			Image_Main->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			if (auto WidgetSlot = Image_Main->Slot)
			{
				if (auto OverlaySlot = Cast<UOverlaySlot>(WidgetSlot))
				{
					OverlaySlot->SetPadding(ImagePadding); // Kind of a hack though. Image MUST be in Overlay for it to work
				}
			}

			Image_Main->SetBrush(Image);
			Image_Main->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UKeyboardButtonSpecialKeyWidget::OnButtonPressed()
{
	BroadcastPressEvent(EButtonPressMethod::ButtonPress);
}

void UKeyboardButtonSpecialKeyWidget::OnButtonReleased()
{
	BroadcastPressEvent(EButtonPressMethod::ButtonRelease);
}