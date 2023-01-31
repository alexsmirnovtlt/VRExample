// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WidgetInteractable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UWidgetInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface that allows to pass Widget Interaction Component and other optional data to UserWidgets.
 * Common practice would be implement them on UserWidgets and call from Actor to Widget or between UserWidgets 
 */
class VIRTUALKEYBOARDS_API IWidgetInteractable
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "WidgetInteractable Interface")
	void SetupWidgetInteraction(class UWidgetInteractionComponent* WidgetInteractionComponent);
	// Should be called from Actor with Widget Component on Construct(). Can be used to update widgets`s visuals (letters on buttons for example)
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "WidgetInteractable Interface")
	void UpdateVisualState();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "WidgetInteractable Interface")
	void SetOptionalData(UClass* ObjectClass, UObject* Object);
};
