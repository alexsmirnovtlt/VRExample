// 2021-2023 Alexander Smirnov


#include "General/Input/ActorComponents/VRInputBindingsActorComponent.h"

#include "TimerManager.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


UVRInputBindingsActorComponent::UVRInputBindingsActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWasSetup = false;
}

void UVRInputBindingsActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
}

void UVRInputBindingsActorComponent::InitInputForLocalVRPlayer(UInputComponent* PlayerInputComponent, ULocalPlayer* LocalPlayer, UObject* InputReceiver)
{
	if (bWasSetup) return;
	if (!ensure(PlayerInputComponent && LocalPlayer && InputReceiver && !InputBindingsClass.IsNull())) return;

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!ensure(EnhancedInput)) return;

	IPlayerInputReceiver* MainReceiverInterface = Cast<IPlayerInputReceiver>(InputReceiver);
	if (!ensure(MainReceiverInterface)) return;
	MainInputReceiver = TScriptInterface<IPlayerInputReceiver>();
	MainInputReceiver.SetInterface(MainReceiverInterface);
	MainInputReceiver.SetObject(InputReceiver);

	auto LoadedInputBindingsClass = InputBindingsClass.LoadSynchronous();
	const auto InputBindingsCDO = Cast<UInputMappingSettings>(LoadedInputBindingsClass->GetDefaultObject());

	if (auto EnhInputSubs = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		EnhInputSubs->ClearAllMappings();
		EnhInputSubs->AddMappingContext(InputBindingsCDO->InputMapping.Get(), 0);
	}

	BindInputActionsForHand(EnhancedInput, false, InputBindingsCDO->LeftHandActions);
	BindInputActionsForHand(EnhancedInput, true, InputBindingsCDO->RightHandActions);
	// Finalize setup
	bWasSetup = true;
}

void UVRInputBindingsActorComponent::BindInputActionsForHand(UEnhancedInputComponent* EnhancedInput, bool bIsRightHand, const FVRControllerMapping& InputActions)
{
	// Axes
	EnhancedInput->BindAction(InputActions.Thumbstick_2DAxis, ETriggerEvent::Triggered, this, &UVRInputBindingsActorComponent::Thumbstick_2DAxis, bIsRightHand);
	EnhancedInput->BindAction(InputActions.Trigger_Axis, ETriggerEvent::Triggered, this, &UVRInputBindingsActorComponent::Trigger_Axis, bIsRightHand);
	EnhancedInput->BindAction(InputActions.Grip_Axis, ETriggerEvent::Triggered, this, &UVRInputBindingsActorComponent::Grip_Axis, bIsRightHand);
	// Presses
	EnhancedInput->BindAction(InputActions.UpperButton_Press, ETriggerEvent::Started, this, &UVRInputBindingsActorComponent::MainButton_Action, bIsRightHand, EButtonActionType::Pressed, true);
	EnhancedInput->BindAction(InputActions.LowerButton_Press, ETriggerEvent::Started, this, &UVRInputBindingsActorComponent::MainButton_Action, bIsRightHand, EButtonActionType::Pressed, false);
	EnhancedInput->BindAction(InputActions.Thumbstick_Press, ETriggerEvent::Started, this, &UVRInputBindingsActorComponent::Thumbstick_Action, bIsRightHand, EButtonActionType::Pressed);
	if(InputActions.Menu_Press)
		EnhancedInput->BindAction(InputActions.Menu_Press, ETriggerEvent::Started, this, &UVRInputBindingsActorComponent::Menu_Action, bIsRightHand, EButtonActionType::Pressed);

	EnhancedInput->BindAction(InputActions.UpperButton_Press, ETriggerEvent::Completed, this, &UVRInputBindingsActorComponent::MainButton_Action, bIsRightHand, EButtonActionType::ReleasedPress, true);
	EnhancedInput->BindAction(InputActions.LowerButton_Press, ETriggerEvent::Completed, this, &UVRInputBindingsActorComponent::MainButton_Action, bIsRightHand, EButtonActionType::ReleasedPress, false);
	EnhancedInput->BindAction(InputActions.Thumbstick_Press, ETriggerEvent::Completed, this, &UVRInputBindingsActorComponent::Thumbstick_Action, bIsRightHand, EButtonActionType::ReleasedPress);
	if(InputActions.Menu_Press)
		EnhancedInput->BindAction(InputActions.Menu_Press, ETriggerEvent::Completed, this, &UVRInputBindingsActorComponent::Menu_Action, bIsRightHand, EButtonActionType::ReleasedPress);

	if (CustomInputParams.bFireTouchEvents)
	{
		EnhancedInput->BindAction(InputActions.Trigger_Touch, ETriggerEvent::Started, this, &UVRInputBindingsActorComponent::Trigger_Touch, bIsRightHand, EButtonActionType::Touched);
		EnhancedInput->BindAction(InputActions.UpperButton_Touch, ETriggerEvent::Started, this, &UVRInputBindingsActorComponent::MainButton_TouchAction, bIsRightHand, EButtonActionType::Touched, true);
		EnhancedInput->BindAction(InputActions.LowerButton_Touch, ETriggerEvent::Started, this, &UVRInputBindingsActorComponent::MainButton_TouchAction, bIsRightHand, EButtonActionType::Touched, false);
		EnhancedInput->BindAction(InputActions.Thumbstick_Touch, ETriggerEvent::Started, this, &UVRInputBindingsActorComponent::Thumbstick_Action, bIsRightHand, EButtonActionType::Touched);

		EnhancedInput->BindAction(InputActions.Trigger_Touch, ETriggerEvent::Completed, this, &UVRInputBindingsActorComponent::Trigger_Touch, bIsRightHand, EButtonActionType::ReleasedTouch);
		EnhancedInput->BindAction(InputActions.UpperButton_Touch, ETriggerEvent::Completed, this, &UVRInputBindingsActorComponent::MainButton_TouchAction, bIsRightHand, EButtonActionType::ReleasedTouch, true);
		EnhancedInput->BindAction(InputActions.LowerButton_Touch, ETriggerEvent::Completed, this, &UVRInputBindingsActorComponent::MainButton_TouchAction, bIsRightHand, EButtonActionType::ReleasedTouch, false);
		EnhancedInput->BindAction(InputActions.Thumbstick_Touch, ETriggerEvent::Completed, this, &UVRInputBindingsActorComponent::Thumbstick_Action, bIsRightHand, EButtonActionType::ReleasedTouch);
	}
}

// INPUT HANDLING

void UVRInputBindingsActorComponent::Thumbstick_2DAxis(const FInputActionInstance& Instance, bool bIsRightHand)
{
	const FVector2D Value = Instance.GetValue().Get<FVector2D>();
	if (auto Receiver = GetTargetInputReceiver(bIsRightHand))
		IPlayerInputReceiver::Execute_Input_Axis_Thumbstick(Receiver,
			Value.X, Value.Y, GetDirectionFromAxes(Value.X, Value.Y, CustomInputParams.ThumbstickDeadzone));
}

void UVRInputBindingsActorComponent::Trigger_Axis(const FInputActionInstance& Instance, bool bIsRightHand)
{
	float& TriggerValue = bIsRightHand ? Axis_Trigger_Right_Value : Axis_Trigger_Left_Value;
	bool& bIsTriggerPressed = bIsRightHand ? bIsTriggerPressed_Right : bIsTriggerPressed_Left;
	
	float Value = Instance.GetValue().Get<float>();
	float RoundedValue = FMath::IsNearlyZero(Value, CustomInputParams.NearZeroTolerance) ? 0.f : Value;
	
	if (TriggerValue == RoundedValue) return;
	else TriggerValue = RoundedValue;

	if (auto Receiver = GetTargetInputReceiver(bIsRightHand))
		IPlayerInputReceiver::Execute_Input_Axis_Trigger(Receiver, TriggerValue);

	if (CustomInputParams.bFireTriggerPressAction)
	{
		bool bIsTriggerPressedNow = TriggerValue >= CustomInputParams.TriggerPressThreshold;
		if (bIsTriggerPressedNow != bIsTriggerPressed)
		{
			if (auto Receiver = GetTargetInputReceiver(bIsRightHand))
				IPlayerInputReceiver::Execute_Input_Trigger_Press(Receiver, bIsTriggerPressedNow);
			bIsTriggerPressed = bIsTriggerPressedNow;
		}
	}
}

void UVRInputBindingsActorComponent::Grip_Axis(const FInputActionInstance& Instance, bool bIsRightHand)
{
	float& GripValue = bIsRightHand ? Axis_Grip_Right_Value : Axis_Grip_Left_Value;
	bool& bIsGripPressed = bIsRightHand ? bIsGripPressed_Right : bIsGripPressed_Left;

	float Value = Instance.GetValue().Get<float>();
	float RoundedValue = FMath::IsNearlyZero(Value, CustomInputParams.NearZeroTolerance) ? 0.f : Value;
	
	if (GripValue == RoundedValue) return;
	else GripValue = RoundedValue;

	if (auto Receiver = GetTargetInputReceiver(bIsRightHand))
		IPlayerInputReceiver::Execute_Input_Axis_Grip(Receiver, GripValue);

	if (CustomInputParams.bFireGripPressAction)
	{
		bool bIsGripPressedNow = GripValue >= CustomInputParams.GripPressThreshold;
		if (bIsGripPressedNow != bIsGripPressed)
		{
			if (auto Receiver = GetTargetInputReceiver(bIsRightHand))
				IPlayerInputReceiver::Execute_Input_Grip_Press(Receiver, bIsGripPressedNow);
			bIsGripPressed = bIsGripPressedNow;
		}
	}
}

void UVRInputBindingsActorComponent::MainButton_Action(const FInputActionInstance& Instance, bool bIsRightHand, EButtonActionType ActionType, bool bIsUpperButton)
{
	if(auto Receiver = GetTargetInputReceiver(bIsRightHand))
		ForwardMainButtonInput(bIsRightHand, ActionType, bIsUpperButton, Receiver); // Press and release events get triggered always

	//UE_LOG(LogTemp, Warning, TEXT("Right: %d, Type: %d"), bIsRightHand ? 1 : 0, (int32)ActionType);

	if (!CustomInputParams.bFireDoubleTapEvents && !CustomInputParams.bFireHoldEvents) return; // No additional logic needs to be applied

	if (ActionType == EButtonActionType::Pressed)
	{ // Start timer to distinct between Single/Double Tap or Hold
		SetLastMainButtonAction(bIsRightHand, bIsUpperButton, ActionType);
		auto Handle = GetTimerHandle(bIsRightHand, bIsUpperButton);
		if (!Handle.IsValid())
		{ // Setup button input timer
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "OnInputTimerEnded", bIsRightHand, bIsUpperButton);
			GetWorld()->GetTimerManager().SetTimer(GetTimerHandle(bIsRightHand, bIsUpperButton), Delegate, CustomInputParams.PressesTimeframe, false);
		}
	}
	else if (ActionType == EButtonActionType::ReleasedPress)
	{
		SetLastMainButtonAction(bIsRightHand, bIsUpperButton, ActionType);
		if(ShouldSkipButtonReleaseTapIncrement(bIsRightHand, bIsUpperButton))
			SetReleaseActionSkipAfterHold(bIsRightHand, bIsUpperButton, false); // Reset skip bool
		else 
			ChangePressesNumber(bIsRightHand, bIsUpperButton, false); // Increment Press
	}
}

void UVRInputBindingsActorComponent::MainButton_TouchAction(const FInputActionInstance& Instance, bool bIsRightHand, EButtonActionType ActionType, bool bIsUpperButton)
{
	if (auto Receiver = GetTargetInputReceiver(bIsRightHand))
	{
		if(bIsUpperButton) IPlayerInputReceiver::Execute_Input_Button_Upper(Receiver, ActionType);
		else IPlayerInputReceiver::Execute_Input_Button_Lower(Receiver, ActionType);
	}
}

void UVRInputBindingsActorComponent::Thumbstick_Action(const FInputActionInstance& Instance, bool bIsRightHand, EButtonActionType ActionType)
{
	if (auto Receiver = GetTargetInputReceiver(bIsRightHand)) IPlayerInputReceiver::Execute_Input_Button_Thumbstick(Receiver, ActionType);
}

void UVRInputBindingsActorComponent::Menu_Action(const FInputActionInstance& Instance, bool bIsRightHand, EButtonActionType ActionType)
{
	if (auto Receiver = GetTargetInputReceiver(bIsRightHand)) IPlayerInputReceiver::Execute_Input_Button_Menu(Receiver, ActionType);
}

void UVRInputBindingsActorComponent::Trigger_Touch(const FInputActionInstance& Instance, bool bIsRightHand, EButtonActionType ActionType)
{
	bool bStartedTouch = ActionType == EButtonActionType::Touched;
	if (auto Receiver = GetTargetInputReceiver(bIsRightHand)) IPlayerInputReceiver::Execute_Input_Trigger_Touch(Receiver, bStartedTouch);
}

// TIMER RELATED FUNCTIONS

FTimerHandle& UVRInputBindingsActorComponent::GetTimerHandle(bool bIsRightHand, bool bIsUpperButton)
{
	if (bIsRightHand && bIsUpperButton) return UpperButtonTimerHandle_Right;
	else if (bIsRightHand && !bIsUpperButton) return LowerButtonTimerHandle_Right;
	else if (!bIsRightHand && bIsUpperButton) return UpperButtonTimerHandle_Left;
	else return LowerButtonTimerHandle_Left;
}

void UVRInputBindingsActorComponent::OnInputTimerEnded(bool bIsRightHand, bool bIsUpperButton)
{
	EButtonActionType actionToFire = EButtonActionType::Touched; // there is no None so this will do
	//UE_LOG(LogTemp, Warning, TEXT("Timer ended. Right: %d, isUpper: %d,  num presses: %d"), bIsRightHand ? 1 : 0, bIsUpperButton ? 1 : 0, (int32)GetPressesNumber(bIsRightHand, bIsUpperButton));
	
	if(GetLastMainButtonAction(bIsRightHand, bIsUpperButton) == EButtonActionType::Pressed)
		SetReleaseActionSkipAfterHold(bIsRightHand, bIsUpperButton, true); // Otherwise button`s release will increment tap count

	if (CustomInputParams.bFireHoldEvents && GetPressesNumber(bIsRightHand, bIsUpperButton) == 0)
		actionToFire = EButtonActionType::Held;

	if (CustomInputParams.bFireDoubleTapEvents)
	{
		if (GetPressesNumber(bIsRightHand, bIsUpperButton) == 0x01) actionToFire = EButtonActionType::SingleTap;
		else if (GetPressesNumber(bIsRightHand, bIsUpperButton) > 0x01) actionToFire = EButtonActionType::DoubleTap;
	}

	GetTimerHandle(bIsRightHand, bIsUpperButton).Invalidate();
	ChangePressesNumber(bIsRightHand, bIsUpperButton, true);

	if (actionToFire != EButtonActionType::Touched)
	{
		if (auto Receiver = GetTargetInputReceiver(bIsRightHand))
			ForwardMainButtonInput(bIsRightHand, actionToFire, bIsUpperButton, Receiver);
	}
}

void UVRInputBindingsActorComponent::SetLastMainButtonAction(bool bIsRightHand, bool bIsUpperButton, EButtonActionType ActionType)
{
	if (bIsRightHand && bIsUpperButton) LastMainButtonAction_Upper_Right = ActionType;
	else if (bIsRightHand && !bIsUpperButton) LastMainButtonAction_Lower_Right = ActionType;
	else if (!bIsRightHand && bIsUpperButton) LastMainButtonAction_Upper_Left = ActionType;
	else LastMainButtonAction_Lower_Left = ActionType;
}

EButtonActionType UVRInputBindingsActorComponent::GetLastMainButtonAction(bool bIsRightHand, bool bIsUpperButton)
{
	if (bIsRightHand && bIsUpperButton) return LastMainButtonAction_Upper_Right;
	else if (bIsRightHand && !bIsUpperButton) return LastMainButtonAction_Lower_Right;
	else if (!bIsRightHand && bIsUpperButton) return LastMainButtonAction_Upper_Left;
	else return LastMainButtonAction_Lower_Left;
}

uint8 UVRInputBindingsActorComponent::GetPressesNumber(bool bIsRightHand, bool bIsUpperButton)
{
	if (bIsRightHand && bIsUpperButton) return NumPresses_UpperButton_Right;
	else if (bIsRightHand && !bIsUpperButton) return NumPresses_LowerButton_Right;
	else if (!bIsRightHand && bIsUpperButton) return NumPresses_UpperButton_Left;
	else return NumPresses_LowerButton_Left;
}

void UVRInputBindingsActorComponent::SetReleaseActionSkipAfterHold(bool bIsRightHand, bool bIsUpperButton, bool bSkip)
{
	if (bIsRightHand && bIsUpperButton) bSkipNextRelease_UpperButton_Right = bSkip;
	else if (bIsRightHand && !bIsUpperButton) bSkipNextRelease_LowerButton_Right = bSkip;
	else if (!bIsRightHand && bIsUpperButton) bSkipNextRelease_UpperButton_Left = bSkip;
	else bSkipNextRelease_LowerButton_Left = bSkip;
}

bool UVRInputBindingsActorComponent::ShouldSkipButtonReleaseTapIncrement(bool bIsRightHand, bool bIsUpperButton)
{
	if (bIsRightHand && bIsUpperButton) return bSkipNextRelease_UpperButton_Right;
	else if (bIsRightHand && !bIsUpperButton) return bSkipNextRelease_LowerButton_Right;
	else if (!bIsRightHand && bIsUpperButton) return bSkipNextRelease_UpperButton_Left;
	else return bSkipNextRelease_LowerButton_Left;
}

void UVRInputBindingsActorComponent::ChangePressesNumber(bool bIsRightHand, bool bIsUpperButton, bool bResetInsteadOfIncrement)
{
	if (bIsRightHand && bIsUpperButton) NumPresses_UpperButton_Right = bResetInsteadOfIncrement ? 0 : ++NumPresses_UpperButton_Right;
	else if (bIsRightHand && !bIsUpperButton) NumPresses_LowerButton_Right = bResetInsteadOfIncrement ? 0 : ++NumPresses_LowerButton_Right;
	else if (!bIsRightHand && bIsUpperButton) NumPresses_UpperButton_Left = bResetInsteadOfIncrement ? 0 : ++NumPresses_UpperButton_Left;
	else NumPresses_LowerButton_Left = bResetInsteadOfIncrement ? 0 : ++NumPresses_LowerButton_Left;
}

void UVRInputBindingsActorComponent::ForwardMainButtonInput(bool bIsRightHand, EButtonActionType ActionType, bool bIsUpperButton, UObject* Receiver)
{
	if (bIsUpperButton) IPlayerInputReceiver::Execute_Input_Button_Upper(Receiver, ActionType);
	else IPlayerInputReceiver::Execute_Input_Button_Lower(Receiver, ActionType);
}

// OTHER FUNCTIONS

UObject* UVRInputBindingsActorComponent::GetTargetInputReceiver(bool bIsRightHand)
{
	auto InputReceiver = IPlayerInputReceiver::Execute_GetInputReceiver(MainInputReceiver.GetObject(), bIsRightHand);
	return InputReceiver.GetObject();
}

EThumbstickArea UVRInputBindingsActorComponent::GetDirectionFromAxes(float X, float Y, float Deadzone)
{
	if (FMath::Abs(X) < Deadzone &&
		FMath::Abs(Y) < Deadzone)
		return EThumbstickArea::Deadzone;

	// TODO just use math and find an angle man
	if (X > 0)
	{
		if (FMath::Abs(Y) < X) return EThumbstickArea::Right;
		else if (Y > 0) return EThumbstickArea::Up;
		else return EThumbstickArea::Down;
	}
	else
	{
		if (FMath::Abs(Y) < FMath::Abs(X)) return EThumbstickArea::Left;
		else if (Y > 0) return EThumbstickArea::Up;
		else return EThumbstickArea::Down;
	}
}