// 2021-2022 Alexander Smirnov


#include "General/Gameplay/Interaction/Player/GrabComponent.h"

#include "Components/ShapeComponent.h"

#include "General/Gameplay/Actors/VRControllerBase.h"

UGrabComponent::UGrabComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bCanEverBeActive = true;
	bUpdatePickableObject = false;
}

void UGrabComponent::SetupComponent(AVRControllerBase* OwningHand, UShapeComponent* Primitive)
{
	if (!bCanEverBeActive) return;
	if (!ensure(OwningHand && Primitive)) return;

	OwningHandController = OwningHand;
	bUpdatePickableObject = true;

	Primitive->OnComponentBeginOverlap.AddDynamic(this, &UGrabComponent::GrabSphere_BeginOverlap);
	Primitive->OnComponentEndOverlap.AddDynamic(this, &UGrabComponent::GrabSphere_EndOverlap);
}

void UGrabComponent::UpdateInteraction(float DeltaTime, bool bTickOnly)
{
	if (!bCanEverBeActive) return;
	if (IsValid(PickedObject.GetObject()))
	{
		IPlayerPickable::Execute_OnHandControllerTick(PickedObject.GetObject(), OwningHandController);
		return;
	}

	UObject* CurrentPickable = NearestPickableObject.GetObject();
	bool bTickCurrentPickable = IsValid(CurrentPickable);
	bool bShouldSkipTick = false; // If new pickable is set, no need to call Proximity Start and Tick in the same frame

	if (!bTickOnly && bUpdatePickableObject) // Finding new (or use the same) current active pickable
	{
		float DistanceToActor = TNumericLimits<float>::Max();
		AActor* NewCurrentPickable = nullptr;
		FVector HandLocation = OwningHandController->GetGrabCollisionPrimitive()->GetComponentLocation();

		for (auto& Item : OverlappedActorsArray)
		{
			const FVector& ActorLocation = IPlayerPickable::Execute_GetWorldLocationForProximityCheck(Item, OwningHandController);
			float DistToThisActor = FVector::DistSquared(HandLocation, ActorLocation);
			if (DistToThisActor < DistanceToActor)
			{
				DistanceToActor = DistToThisActor;
				NewCurrentPickable = Item;
			}
		}

		bool bEndProximityOnCurrentPickable = false;
		bool bShouldSetCurrentPickableToNone = false;

		if (!NewCurrentPickable) // No valid actor in proximity
		{
			if (bTickCurrentPickable)
			{
				bTickCurrentPickable = false;
				bEndProximityOnCurrentPickable = true;
			}
			bShouldSetCurrentPickableToNone = true;
		}
		else // New pickable is a valid actor, can be the same actor or a different one
		{
			if (bTickCurrentPickable && NewCurrentPickable != CurrentPickable)
				bEndProximityOnCurrentPickable = true; // New actor is a different one
			// else do nothing, bTickCurrentPickable is true, no need to update anything
		}

		if (bEndProximityOnCurrentPickable)
			IPlayerPickable::Execute_OnHandProximity_End(NearestPickableObject.GetObject(), OwningHandController, nullptr);

		if (bShouldSetCurrentPickableToNone)
		{
			bTickCurrentPickable = false;
			if (IsValid(NearestPickableObject.GetObject()))
			{
				NearestPickableObject.SetObject(nullptr);
				NearestPickableObject.SetInterface(nullptr);
			}
		}

		if (NewCurrentPickable) // Now handling new pickable	
		{
			bShouldSkipTick = true;
			bTickCurrentPickable = true;

			if (NewCurrentPickable != CurrentPickable)
			{
				NearestPickableObject.SetObject(NewCurrentPickable);
				IPlayerPickable::Execute_OnHandProximity_Start(NewCurrentPickable, OwningHandController, nullptr);
			}
		}
	}

	if (bTickCurrentPickable && !bShouldSkipTick)
	{
		IPlayerPickable::Execute_OnHandControllerTick(CurrentPickable, OwningHandController);
	}
}

void UGrabComponent::SetKeepUpdatingInteraction(bool bEnableUpdate, bool bForceStateChange)
{
	if (bCanEverBeActive) bUpdatePickableObject = bEnableUpdate;
	if (!bEnableUpdate && bForceStateChange)
	{
		bool bDropped;
		DropPickedObject(bDropped);
		if (IsValid(NearestPickableObject.GetObject()))
		{
			IPlayerPickable::Execute_OnDrop(NearestPickableObject.GetObject(), OwningHandController);
			NearestPickableObject.SetObject(nullptr);
			NearestPickableObject.SetInterface(nullptr);
		}
	}
}

void UGrabComponent::GrabNearestPickable(bool& bWasPicked)
{
	bWasPicked = false;
	ensure(!IsValid(PickedObject.GetObject()));
	if (IsValid(NearestPickableObject.GetObject()))
	{
		IPlayerPickable::Execute_OnGrab(NearestPickableObject.GetObject(), OwningHandController);
		PickedObject = NearestPickableObject;
		bWasPicked = true;
	}
}

void UGrabComponent::DropPickedObject(bool& bWasDropped)
{
	bWasDropped = false;
	if (IsValid(PickedObject.GetObject()))
	{
		IPlayerPickable::Execute_OnDrop(PickedObject.GetObject(), OwningHandController);
		PickedObject.SetObject(nullptr);
		PickedObject.SetInterface(nullptr);
		bWasDropped = true;
	}
}

void UGrabComponent::GrabSphere_BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->Implements<UPlayerPickable>()) return;
	OverlappedActorsArray.AddUnique(OtherActor);
}

void UGrabComponent::GrabSphere_EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->Implements<UPlayerPickable>()) return;

	OverlappedActorsArray.RemoveSingle(OtherActor);
	//IPlayerPickable::Execute_OnHandProximity_End(OtherActor, OwningHandController, OverlappedComp);
}