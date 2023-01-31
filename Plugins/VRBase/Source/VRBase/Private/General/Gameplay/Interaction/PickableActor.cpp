// 2021-2022 Alexander Smirnov


#include "General/Gameplay/Interaction/PickableActor.h"

#include "General/Gameplay/Interaction/Player/GrabComponent.h"
#include "General/Gameplay/Actors/VRControllerBase.h"

void APickableActor::OnHandProximity_Start_Implementation(AVRControllerBase* HandController, USceneComponent* CollidedComponent)
{
	if (IsHandHoldingAnObject(HandController)) return;
	ensure(HoveredHandState);
	UpdateHoveredState(true);
	if(HoveredHandState) HandController->ChangeState(HoveredHandState, true);
}

void APickableActor::OnHandProximity_End_Implementation(AVRControllerBase* HandController, USceneComponent* CollidedComponent)
{
	if (IsHandHoldingAnObject(HandController)) return;

	UpdateHoveredState(false);
	HandController->ChangeToDefaultState();
}

void APickableActor::OnGrab_Implementation(AVRControllerBase* HandController)
{
	ensure(GrabbedHandState);

	// First try to drop this actor off another hand if able
	if (HoldingHand)
	{
		auto PickedObject = HoldingHand->GetGrabComponent()->GetPickedObject();
		if (IsValid(PickedObject.GetObject()) && PickedObject.GetObject() == this)
		{
			bSkipStateChangeOnNextDrop = true;
			bool bWasDropped;
			HoldingHand->GetGrabComponent()->DropPickedObject(bWasDropped);
		}
	}

	HoldingHand = HandController;

	if (!bSkipStateChangeOnNextDrop)
	{
		UpdateHoveredState(false);
		PickedStateChanged(true);
	}
	else bSkipStateChangeOnNextDrop = false;

	if(GrabbedHandState) HandController->ChangeState(GrabbedHandState, true);

	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, WeldSimulatedBodiesOnAttach);
	AttachToActor(HoldingHand, AttachmentRules);

	HoldingHand->SetHandVisibility(false);
}

void APickableActor::OnDrop_Implementation(AVRControllerBase* HandController)
{
	ensure(HandController == HoldingHand);

	DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, InCallModifyOnDetach));
	if (HoveredHandState) HandController->ChangeState(HoveredHandState, true);
	HoldingHand->SetHandVisibility(true);
	
	if (!bSkipStateChangeOnNextDrop)
	{
		PickedStateChanged(false);
		UpdateHoveredState(true);
	}

	HoldingHand = nullptr;
}

bool APickableActor::IsGrabbed_Implementation() const
{
	return IsValid(HoldingHand);
}

bool APickableActor::IsHandHoldingAnObject(AVRControllerBase* Hand)
{
	auto PickedObject = Hand->GetGrabComponent()->GetPickedObject();
	return IsValid(PickedObject.GetObject());
}

void APickableActor::UpdateHoveredState(bool bSetHovered)
{
	// Counting hovered hands to prevent redundant Hovered State changes

	HoveredHandsNum = bSetHovered ? HoveredHandsNum + 1 : HoveredHandsNum - 1;

	if(bSetHovered && HoveredHandsNum == 1) SetHighligted(true);
	else if(!bSetHovered && HoveredHandsNum == 0) SetHighligted(false);

	ensure(HoveredHandsNum >= 0 && HoveredHandsNum < 3);	
}