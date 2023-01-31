// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//#include "Delegates/IDelegateInstance.h"
#include "VirtualKeyboardPanelBaseWidget.generated.h"

class UKeyboardButtonBaseWidget;

/**
 * Collection of Buttons. Gets created and destroyed by Keyboard class
 * Used primarily for language switching or to switch between symbols/characters
 */
UCLASS(Abstract)
class VIRTUALKEYBOARDS_API UVirtualKeyboardPanelBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;

public:

	void SetupKeyboardPanel(UVirtualKeyboardBaseWidget* OwningKeyboard);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VirtualKeyboardPanelBaseWidget")
	UWidgetInteractionComponent* GetWidgetInteraction() const { return KeyboardParent->GetWidgetInteractionComponent(); };

	const TArray<UKeyboardButtonBaseWidget*>& GetButtons() const { return ButtonsArray; };

protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<UKeyboardButtonBaseWidget*> ButtonsArray;
	UPROPERTY()
	UVirtualKeyboardBaseWidget* KeyboardParent;
};
