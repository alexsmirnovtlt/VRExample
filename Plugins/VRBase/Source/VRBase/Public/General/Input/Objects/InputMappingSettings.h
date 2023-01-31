// 2021-2023 Alexander Smirnov

#pragma once

#include "Engine/DataAsset.h"
#include "InputMappingSettings.generated.h"

class UInputAction;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FVRControllerMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Axes")
	TObjectPtr<const UInputAction> Thumbstick_2DAxis;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Axes")
	TObjectPtr<const UInputAction> Trigger_Axis;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Axes")
	TObjectPtr<const UInputAction> Grip_Axis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presses")
	TObjectPtr<const UInputAction> Thumbstick_Press;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presses")
	TObjectPtr<const UInputAction> UpperButton_Press;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presses")
	TObjectPtr<const UInputAction> LowerButton_Press;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presses")
	TObjectPtr<const UInputAction> Menu_Press;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Touches")
	TObjectPtr<const UInputAction> Thumbstick_Touch;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Touches")
	TObjectPtr<const UInputAction> Trigger_Touch;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Touches")
	TObjectPtr<const UInputAction> UpperButton_Touch;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Touches")
	TObjectPtr<const UInputAction> LowerButton_Touch;
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class UInputMappingSettings : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<const UInputMappingContext> InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVRControllerMapping LeftHandActions;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVRControllerMapping RightHandActions;
};
