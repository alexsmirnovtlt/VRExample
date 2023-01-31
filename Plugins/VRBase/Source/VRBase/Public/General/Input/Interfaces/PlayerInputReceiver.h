// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInputReceiver.generated.h"

UENUM(BlueprintType)
enum class EButtonActionType : uint8 {
	Pressed = 0 UMETA(DisplayName = "Pressed"),
	ReleasedPress = 1 UMETA(DisplayName = "Released Press"),
	Touched = 2 UMETA(DisplayName = "Touched"),
	ReleasedTouch = 3 UMETA(DisplayName = "Released Touch"),
	SingleTap = 5 UMETA(DisplayName = "Single Tap"),
	DoubleTap = 6 UMETA(DisplayName = "Double Tap"),
	Held = 7 UMETA(DisplayName = "Hold")
};

UENUM(BlueprintType)
enum class EThumbstickArea : uint8 {
	Deadzone = 0 UMETA(DisplayName = "Deadzone"),
	Up = 1 UMETA(DisplayName = "Up"),
	Right = 2 UMETA(DisplayName = "Right"),
	Down = 3 UMETA(DisplayName = "Down"),
	Left = 4 UMETA(DisplayName = "Left")
};

UINTERFACE(MinimalAPI, BlueprintType)
class UPlayerInputReceiver : public UInterface
{
	GENERATED_BODY()
};

/**
 * Ability to receive and consume input from the Player without any need to bind events to any InputComponent
 */
class VRBASE_API IPlayerInputReceiver
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | IPlayerInputReceiver | Axes")
	UPARAM(DisplayName = "Consume Input") bool Input_Axis_Thumbstick(float Horizontal, float Vertical, EThumbstickArea ThumbstickArea);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | IPlayerInputReceiver | Axes")
	UPARAM(DisplayName = "Consume Input") bool Input_Axis_Trigger(float Value);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | IPlayerInputReceiver | Axes")
	UPARAM(DisplayName = "Consume Input") bool Input_Axis_Grip(float Value);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | IPlayerInputReceiver | Actions")
	UPARAM(DisplayName = "Consume Input") bool Input_Button_Upper(EButtonActionType ActionType);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | IPlayerInputReceiver | Actions")
	UPARAM(DisplayName = "Consume Input") bool Input_Button_Lower(EButtonActionType ActionType);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | IPlayerInputReceiver | Actions")
	UPARAM(DisplayName = "Consume Input") bool Input_Button_Thumbstick(EButtonActionType ActionType);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | IPlayerInputReceiver | Actions")
	UPARAM(DisplayName = "Consume Input") bool Input_Button_Menu(EButtonActionType ActionType);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | IPlayerInputReceiver | Actions - Other")
	UPARAM(DisplayName = "Consume Input") bool Input_Trigger_Touch(bool bTouched);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | IPlayerInputReceiver | Actions - Other")
	UPARAM(DisplayName = "Consume Input") bool Input_Trigger_Press(bool bPressed);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | IPlayerInputReceiver | Actions - Other")
	UPARAM(DisplayName = "Consume Input") bool Input_Grip_Press(bool bPressed);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | IPlayerInputReceiver | Input Forwarding")
	TScriptInterface<IPlayerInputReceiver> GetInputReceiver(bool IsRightHand) const;
	virtual TScriptInterface<IPlayerInputReceiver> GetInputReceiver_Implementation(bool IsRightHand) const { return nullptr; };
};
