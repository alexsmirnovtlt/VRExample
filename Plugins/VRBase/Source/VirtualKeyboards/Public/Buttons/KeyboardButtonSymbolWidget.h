// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buttons/KeyboardButtonBaseWidget.h"
#include "KeyboardButtonSymbolWidget.generated.h"

/**
 * Button that contains a letter that broadcasts press event on pointer release 
 */
UCLASS()
class VIRTUALKEYBOARDS_API UKeyboardButtonSymbolWidget : public UKeyboardButtonBaseWidget
{
	GENERATED_BODY()

	UKeyboardButtonSymbolWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void OnWidgetRebuilt() override;

public:
	virtual void UpdateVisuals() override;
	// Changes state between Regular letter and Capital if able
	UFUNCTION(BlueprintCallable, Category = "KeyboardButtonSymbolWidget")
	void SwitchState(bool bRegular);
	UFUNCTION(BlueprintCallable, Category = "KeyboardButtonSymbolWidget")
	void ToggleState(bool& IsRegular);

public:
	const FString& GetSymbolAsString() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Components")
	class UTextBlock* TextBlock_Symbol;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button Base Settings")
	FText RegularSymbol;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button Base Settings")
	FText CapitalSymbol;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button Base Settings")
	bool bCanSwitchBetweenCapitalAndRegular = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button Base Settings")
	bool bRegularIsDefault = true;

	bool bIsInRegularState;

private:
	void ChangeText(const FText& NewText);

	UFUNCTION()
	void OnButtonReleased();
};
