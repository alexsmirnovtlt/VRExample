// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "General/Interfaces/PlayerPickable.h"
#include "PickableActor.generated.h"

class UVRControllerState;
class AVRControllerBase;

UCLASS()
class VRBASE_API APickableActor : public AActor, public IPlayerPickable
{
	GENERATED_BODY()

public: // IPlayerPickable
	virtual void OnHandProximity_Start_Implementation(AVRControllerBase* HandController, USceneComponent* CollidedComponent) override;
	virtual void OnHandProximity_End_Implementation(AVRControllerBase* HandController, USceneComponent* CollidedComponent) override;
	virtual void OnGrab_Implementation(AVRControllerBase* HandController) override;
	virtual void OnDrop_Implementation(AVRControllerBase* HandController) override;
	virtual bool IsGrabbed_Implementation() const override;
	// ~ IPlayerPickable

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Customizable")
	TSubclassOf<UVRControllerState> HoveredHandState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Customizable")
	TSubclassOf<UVRControllerState> GrabbedHandState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Customizable - Physics")
	bool WeldSimulatedBodiesOnAttach = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Customizable - Physics")
	bool InCallModifyOnDetach = false; // ? see FDetachmentTransformRules constructor

	UPROPERTY(BlueprintReadOnly)
	AVRControllerBase* HoldingHand;

	void UpdateHoveredState(bool bSetHovered);

	bool bSkipStateChangeOnNextDrop = false; // If hand picked this actor from another hand, no need to change states from dropped to picked again
	uint8 HoveredHandsNum = 0; // Prevents firing SetHighligted() state changes when another hand is hovered over this actor

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | PlayerPickable")
	void SetHighligted(bool bHighlight);
	// Either picked or dropped
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | PlayerPickable")
	void PickedStateChanged(bool bIsPickedNow);

	// Just a utility for quicker validity check on a PickedObject in hand
	UFUNCTION(BlueprintCallable, Category = "VRBase | Interaction | Pickables")
	bool IsHandHoldingAnObject(AVRControllerBase* Hand);
};
