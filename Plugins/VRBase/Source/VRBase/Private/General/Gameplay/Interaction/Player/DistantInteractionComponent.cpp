// 2021-2022 Alexander Smirnov


#include "General/Gameplay/Interaction/Player/DistantInteractionComponent.h"

#include "Components/WidgetInteractionComponent.h"
#include "Runtime/Launch/Resources/Version.h"

#include "General/Gameplay/Pawns/VRPawnBase.h"
#include "General/Gameplay/Actors/VRControllerBase.h"
#include "General/Gameplay/Objects/VRControllerState.h"

UDistantInteractionComponent::UDistantInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	HandInteractableObjectType = ECollisionChannel::ECC_WorldDynamic;
	bCanEverBeActive = true;
}

void UDistantInteractionComponent::SetupComponent(AVRControllerBase* OwningHand, UWidgetInteractionComponent* WidgetInteractionComp)
{
	if (!bCanEverBeActive) return;
	if (!(ensure(OwningHand && WidgetInteractionComp))) return;

	bUpdateInteractableObject = true;
	OwningHandController = OwningHand;
	WidgetInteraction = WidgetInteractionComp;
}

void UDistantInteractionComponent::UpdateInteraction(float DeltaTime, bool bTickOnly)
{
	if (!bCanEverBeActive) return;

	FHitResult HitResult;
	OwningHandController->DoAimLineTrace(HandInteractableObjectType, InteractableObjectsCheckDistance, HitResult);

	if (bTickOnly)
	{
		if(InteractableObject.GetObject())
			IPlayerInteractable::Execute_InteractionTick(InteractableObject.GetObject(), OwningHandController, HitResult);
		return;
	}

	if (HitResult.bBlockingHit)
	{
		if (!HitResult.GetActor() ||
			!HitResult.GetActor()->Implements<UPlayerInteractable>()) {
			InvalidateInteractableObject(); return;
		}

		AActor* HitActor = HitResult.GetActor();

		// Distance check. If we hit an Interactable object from afar, check for Interaction Distance Override. It may want to be unreachable from that distance
		float InteractionDistance = IPlayerInteractable::Execute_GetInteractionDistance(HitActor, OwningHandController);
		// This check will be ignored if GetInteractionDistance() returns default value (0)
		if (InteractionDistance < 0.f || (InteractionDistance > 0.f && HitResult.Distance > InteractionDistance)) { InvalidateInteractableObject(); return; }

		if (IPlayerInteractable::Execute_IsUI(HitActor))
			if(auto ControllerState = OwningHandController->GetControllerState())
				if(ControllerState->IsUIInteractionAllowed()) WidgetInteraction->SetCustomHitResult(HitResult);

		if (IsValid(InteractableObject.GetObject()))
		{
			if (Cast<UObject>(HitActor) == InteractableObject.GetObject())
			{
				// Same object as before, just execute tick
				IPlayerInteractable::Execute_InteractionTick(InteractableObject.GetObject(), OwningHandController, HitResult);
			}
			else
			{
				InvalidateInteractableObject();
				InteractableObject.SetObject(HitActor);
				IPlayerInteractable::Execute_InteractionStarted(InteractableObject.GetObject(), OwningHandController, HitResult);
			}
		}
		else
		{
			InteractableObject.SetObject(HitActor);
			IPlayerInteractable::Execute_InteractionStarted(InteractableObject.GetObject(), OwningHandController, HitResult);
		}
	}
	else InvalidateInteractableObject();
}

void UDistantInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	InvalidateInteractableObject();
}

void UDistantInteractionComponent::SetKeepUpdatingInteraction(bool bEnableUpdate, bool bForceStateChange)
{
	if (bCanEverBeActive) bUpdateInteractableObject = bEnableUpdate;
	if (!bEnableUpdate && bForceStateChange && IsValid(InteractableObject.GetObject()))
	{
		IPlayerInteractable::Execute_InteractionEnded(InteractableObject.GetObject(), OwningHandController);
		InteractableObject.SetObject(nullptr);
		InteractableObject.SetInterface(nullptr);
	}
}

void UDistantInteractionComponent::InvalidateInteractableObject()
{
	ClearSlateWidgetReference();

	if (!IsValid(InteractableObject.GetObject())) return;

	IPlayerInteractable::Execute_InteractionEnded(InteractableObject.GetObject(), OwningHandController);
	InteractableObject.SetObject(nullptr);
}

void UDistantInteractionComponent::ClearSlateWidgetReference()
{
	if (WidgetInteraction && WidgetInteraction->GetHoveredWidgetComponent()) // means that CustomHitResult is a valid widget
	{
		WidgetInteraction->SetCustomHitResult(FHitResult());

		/*FWidgetPath EmptyWidgetPath;
		FPointerEvent PointerEvent(
			WidgetInteraction->VirtualUserIndex,
			WidgetInteraction->PointerIndex,
			FVector2D(),
			FVector2D(),
			TSet<FKey>(),
			FKey(),
			0.0f,
			FModifierKeysState());

		FSlateApplication::Get().RoutePointerMoveEvent(EmptyWidgetPath, PointerEvent, false);*/
	}
}

void UDistantInteractionComponent::ForceUpdateInteractableObject(const TScriptInterface<IPlayerInteractable>& NewInteractableObject, bool InvalidatePrevious)
{
	if (InvalidatePrevious) InvalidateInteractableObject();
	InteractableObject = NewInteractableObject;
}