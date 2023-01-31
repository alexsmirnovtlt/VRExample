// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Interfaces/WidgetInteractable.h"

#include "VirtualKeyboardBaseWidget.generated.h"

class UWidgetInteractionComponent;

/**
 * Base class for every virtual keyboard. Automatically handles button presses
 */
UCLASS(Abstract)
class VIRTUALKEYBOARDS_API UVirtualKeyboardBaseWidget : public UUserWidget, public IWidgetInteractable
{
	GENERATED_BODY()
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;

public:

	// IWidgetInteractable Interface
	void SetupWidgetInteraction_Implementation(UWidgetInteractionComponent* WidgetInteractionComponent) override;
	void UpdateVisualState_Implementation() override;
	// ~ IWidgetInteractable Interface

	// 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Virtual Keyboard Base")
	UWidgetInteractionComponent* GetWidgetInteractionComponent() const { return WidgetInteraction.IsValid() ? WidgetInteraction.Get() : nullptr; };

	void OnKeyboardButtonPressed(EButtonPressMethod::Type PressType, UKeyboardButtonBaseWidget* ButtonWidget); // Made public so other keyboard panels may subscribe to it 

	UFUNCTION(BlueprintCallable, Category = "Virtual Keyboard Base")
	UVirtualKeyboardPanelBaseWidget* CreatePanel(TSubclassOf<UVirtualKeyboardPanelBaseWidget> PanelClass);

protected:
	// Will be called if some custom button derived from UKeyboardButtonBaseWidget was clicked
	UFUNCTION(BlueprintNativeEvent, Category = "Virtual Keyboard Base")
	void OnButtonPressed_Custom(const UKeyboardButtonBaseWidget* ButtonWidget);
	void OnButtonPressed_Custom_Implementation(const UKeyboardButtonBaseWidget* ButtonWidget) {}; // Should be overridden in BPs if needed
	UFUNCTION(BlueprintNativeEvent, Category = "Virtual Keyboard Base")
	void OnButtonPressed_Symbol(const FString& Symbol);
	void OnButtonPressed_Symbol_Implementation(const FString& Symbol);
	UFUNCTION(BlueprintNativeEvent, Category = "Virtual Keyboard Base")
	void OnButtonPressed_KeyboardKey(const EButtonPressMethod::Type PressType, const FKey& Key);
	void OnButtonPressed_KeyboardKey_Implementation(const EButtonPressMethod::Type PressType, const FKey& Key);

	UFUNCTION(BlueprintNativeEvent, Category = "Virtual Keyboard Base")
	UPanelWidget* GetPanelsParent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Virtual Keyboard Base")
	TArray<TSoftClassPtr<class UVirtualKeyboardPanelBaseWidget>> KeyboardPanels;

	UPROPERTY()
	TArray<class UKeyboardButtonBaseWidget*> MainButtonsArray;
	UPROPERTY()
	TArray<class UKeyboardButtonBaseWidget*> AdditionalButtonsArray;

	TWeakObjectPtr<UWidgetInteractionComponent> WidgetInteraction;

private:
	void FillDataArrays();
	void SubscribeAllButtonsToButtonPress();
};
