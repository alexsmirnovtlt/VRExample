// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buttons/KeyboardButtonBaseWidget.h"
#include "KeyboardButtonSpecialKeyWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class VIRTUALKEYBOARDS_API UKeyboardButtonSpecialKeyWidget : public UKeyboardButtonBaseWidget
{
	GENERATED_BODY()

	UKeyboardButtonSpecialKeyWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void OnWidgetRebuilt() override;

public:
	const FKey& GetKeyboardKey() const { return InputKey; };
	virtual void UpdateVisuals() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Components")
	class UTextBlock* TextBlock_Main;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Components")
	class UImage* Image_Main;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button Base Settings")
	FKey InputKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button Base Settings")
	FSlateBrush Image;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button Base Settings")
	FMargin ImagePadding;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button Base Settings")
	FText Text;

private:
	UFUNCTION()
	void OnButtonPressed();
	UFUNCTION()
	void OnButtonReleased();
};
