// 2021-2023 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "General/Input/Interfaces/PlayerInputReceiver.h"
#include "General/Input/Objects/InputMappingSettings.h"

#include "VRInputBindingsActorComponent.generated.h"

class ULocalPlayer;
class UInputComponent;
class UInputMappingSettings;
class UEnhancedInputComponent;

struct FInputActionInstance;
struct FVRControllerMapping;

USTRUCT(BlueprintType)
struct FCustomInputMappingParams
{
	GENERATED_BODY()

	FCustomInputMappingParams()
	{
		bFireTriggerPressAction = true;
		bFireGripPressAction = true;

		bFireTouchEvents = true;
		bFireHoldEvents = false;
		bFireDoubleTapEvents = false;

		NearZeroTolerance = 0.01f;
		ThumbstickDeadzone = 0.1f;
		TriggerPressThreshold = 0.9f;
		GripPressThreshold = 0.9f;

		PressesTimeframe = .65f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFireTriggerPressAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFireGripPressAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFireTouchEvents;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFireHoldEvents;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFireDoubleTapEvents;

	// Prevents grip and trigger axis events when player is not even touching them
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NearZeroTolerance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThumbstickDeadzone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TriggerPressThreshold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GripPressThreshold;

	// Used for Double and Single Tap and Hold
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PressesTimeframe;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), hidecategories = ("Variable", "Tags", "ComponentTick", "ComponentReplication", "Activation", "Cooking", "AssetUserData", "Collision", "Sockets", "Events"))
class VRBASE_API UVRInputBindingsActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UVRInputBindingsActorComponent();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void InitInputForLocalVRPlayer(UInputComponent* PlayerInputComponent, ULocalPlayer* LocalPlayer, UObject* InputReceiver);
	FORCEINLINE EThumbstickArea GetDirectionFromAxes(float X, float Y, float Deadzone);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Setup")
	TSoftClassPtr<UInputMappingSettings> InputBindingsClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Setup")
	FCustomInputMappingParams CustomInputParams;

	void BindInputActionsForHand(UEnhancedInputComponent* EnhancedInput, bool bIsRightHand, const FVRControllerMapping& InputActions);

	void Thumbstick_2DAxis(const FInputActionInstance& Instance, bool bIsRightHand);
	void Trigger_Axis(const FInputActionInstance& Instance, bool bIsRightHand);
	void Grip_Axis(const FInputActionInstance& Instance, bool bIsRightHand);

	void MainButton_Action(const FInputActionInstance& Instance, bool bIsRightHand, EButtonActionType ActionType, bool bIsUpperButton);
	void Thumbstick_Action(const FInputActionInstance& Instance, bool bIsRightHand, EButtonActionType ActionType);
	void Menu_Action(const FInputActionInstance& Instance, bool bIsRightHand, EButtonActionType ActionType);

	void MainButton_TouchAction(const FInputActionInstance& Instance, bool bIsRightHand, EButtonActionType ActionType, bool bIsUpperButton);
	void Trigger_Touch(const FInputActionInstance& Instance, bool bIsRightHand, EButtonActionType ActionType);

private:
	TScriptInterface<IPlayerInputReceiver> MainInputReceiver;
	UObject* GetTargetInputReceiver(bool bIsRightHand);

	float Axis_Trigger_Right_Value = 0.f;
	float Axis_Trigger_Left_Value = 0.f;
	float Axis_Grip_Right_Value = 0.f;
	float Axis_Grip_Left_Value = 0.f;

	bool bIsTriggerPressed_Right = false;
	bool bIsTriggerPressed_Left = false;
	bool bIsGripPressed_Right = false;
	bool bIsGripPressed_Left = false;

	bool bWasSetup;

	// Hold and Double press vars and functns
	FTimerHandle UpperButtonTimerHandle_Left;
	FTimerHandle LowerButtonTimerHandle_Left;
	FTimerHandle UpperButtonTimerHandle_Right;
	FTimerHandle LowerButtonTimerHandle_Right;

	uint8 NumPresses_UpperButton_Left;
	uint8 NumPresses_LowerButton_Left;
	uint8 NumPresses_UpperButton_Right;
	uint8 NumPresses_LowerButton_Right;

	EButtonActionType LastMainButtonAction_Upper_Left;
	EButtonActionType LastMainButtonAction_Lower_Left;
	EButtonActionType LastMainButtonAction_Upper_Right;
	EButtonActionType LastMainButtonAction_Lower_Right;

	bool bSkipNextRelease_UpperButton_Left = false; // If HOLD happened, this button`s RELEASE for tap counting must be skipped
	bool bSkipNextRelease_LowerButton_Left = false;
	bool bSkipNextRelease_UpperButton_Right = false;
	bool bSkipNextRelease_LowerButton_Right = false;

	//FORCEINLINE void SetupButtonInputTimer(bool bIsRightHand, bool bIsUpperButton);
	FORCEINLINE uint8 GetPressesNumber(bool bIsRightHand, bool bIsUpperButton);
	FORCEINLINE void SetLastMainButtonAction(bool bIsRightHand, bool bIsUpperButton, EButtonActionType ActionType);
	FORCEINLINE EButtonActionType GetLastMainButtonAction(bool bIsRightHand, bool bIsUpperButton);
	FORCEINLINE void SetReleaseActionSkipAfterHold(bool bIsRightHand, bool bIsUpperButton, bool bSkip);
	FORCEINLINE bool ShouldSkipButtonReleaseTapIncrement(bool bIsRightHand, bool bIsUpperButton);
	FORCEINLINE void ChangePressesNumber(bool bIsRightHand, bool bIsUpperButton, bool bResetInsteadOfIncrement);
	FORCEINLINE FTimerHandle& GetTimerHandle(bool bIsRightHand, bool bIsUpperButton);
	FORCEINLINE void ForwardMainButtonInput(bool bIsRightHand, EButtonActionType ActionType, bool bIsUpperButton, UObject* Receiver);

	UFUNCTION()
	void OnInputTimerEnded(bool bIsRightHand, bool bIsUpperButton);
};
