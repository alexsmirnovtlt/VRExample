// 2021-2022 Alexander Smirnov


#include "Gameplay/Player/ActorComponents/NonVRCharacterMovementComponent.h"

#include "GameFramework/Character.h"

UNonVRCharacterMovementComponent::UNonVRCharacterMovementComponent()
{
	MaxStepHeight = 15.f;
	MaxWalkSpeed = 200.0f;
	MaxSprintSpeed = 400.f;
	MaxWalkSpeedCrouched = 150.f;

	bCanWalkOffLedgesWhenCrouching = true;
	NavAgentProps.bCanCrouch = true;
}

float UNonVRCharacterMovementComponent::GetMaxSpeed() const
{
	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
	case MOVE_Falling:
		if (IsCrouching()) return MaxWalkSpeedCrouched;
		else return RequestToStartSprinting ? MaxSprintSpeed : MaxWalkSpeed;
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		return MaxCustomMovementSpeed;
	case MOVE_None:
	default:
		return 0.f;
	}
}

void UNonVRCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	RequestToStartSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

FNetworkPredictionData_Client* UNonVRCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		auto MutableThis = const_cast<UNonVRCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionDataCharacter_Client(*this);
	}

	return ClientPredictionData;
}

void UNonVRCharacterMovementComponent::FGDSavedMove::Clear()
{
	Super::Clear();
	SavedRequestToStartSprinting = 0;
}

uint8 UNonVRCharacterMovementComponent::FGDSavedMove::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	if (SavedRequestToStartSprinting) Result |= FLAG_Custom_0;
	return Result;
}

bool UNonVRCharacterMovementComponent::FGDSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	if (SavedRequestToStartSprinting != ((FGDSavedMove*)&NewMove)->SavedRequestToStartSprinting) return false;
	else return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void UNonVRCharacterMovementComponent::FGDSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	if (UNonVRCharacterMovementComponent* CharacterMovement = Cast<UNonVRCharacterMovementComponent>(Character->GetCharacterMovement()))
		SavedRequestToStartSprinting = CharacterMovement->RequestToStartSprinting;
}