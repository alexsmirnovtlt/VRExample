// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KeyboardButtonBaseWidget.generated.h"

UENUM()
enum EVirtualKeyboardButtonType
{
	UndefinedButton,
	Symbol,
	SpecialKey
};

DECLARE_EVENT_TwoParams(UKeyboardButtonBaseWidget, FKeyPressedEvent, EButtonPressMethod::Type, UKeyboardButtonBaseWidget*);

/**
 * Base button class of the virtual keyboard
 */
UCLASS(Abstract)
class VIRTUALKEYBOARDS_API UKeyboardButtonBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void UpdateVisuals();

	FKeyPressedEvent& OnKeyPressedEvent() { return KeyPressedEvent; };
	EVirtualKeyboardButtonType GetButtonType() { return ButtonType; };

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Components")
	class UButton* Button_Main;
	UPROPERTY(/*EditAnywhere, BlueprintReadOnly, Category = "Button Base Settings"*/)
	TEnumAsByte<EVirtualKeyboardButtonType> ButtonType;

	UFUNCTION(BlueprintCallable, Category = "Button Base")
	void BroadcastPressEvent(EButtonPressMethod::Type PressType) { KeyPressedEvent.Broadcast(PressType, this); };

	// TEMP Styling settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TEMP Styling Settings")
	FButtonStyle ButtonStyle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TEMP Styling Settings")
	bool bUseCustomStyle = false;
	// TEMP Styling settings

private:
	FKeyPressedEvent KeyPressedEvent;
};
