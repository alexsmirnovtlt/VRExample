// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "General/Interfaces/PlayerPickable.h"

#include "GrabComponent.generated.h"

/**
 * 
 */
UCLASS(hidecategories = ("Variable", "Tags", "ComponentTick", "ComponentReplication", "Activation", "Cooking", "AssetUserData", "Collision", "Sockets", "Events"))
class VRBASE_API UGrabComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UGrabComponent();

public:
	void SetupComponent(AVRControllerBase* OwningHand, class UShapeComponent* Primitive);
	virtual void UpdateInteraction(float DeltaTime, bool bTickOnly);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VR Base | Controller | Grab")
	const TScriptInterface<IPlayerPickable>& GetNearestPickable() const { return NearestPickableObject; };
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VR Base | Controller | Grab")
	const TScriptInterface<IPlayerPickable>& GetPickedObject() const { return PickedObject; };
	UFUNCTION(BlueprintCallable, Category = "VR Base | Controller | Grab")
	void SetKeepUpdatingInteraction(bool bActive, bool bForceStateChange = false);
	UFUNCTION(BlueprintCallable, Category = "VR Base | Controller | Grab")
	void GrabNearestPickable(bool& bWasPicked);
	UFUNCTION(BlueprintCallable, Category = "VR Base | Controller | Grab" )
	void DropPickedObject(bool& bWasDropped);

protected:
	// If true will trigger IPlayerPickable interface calls on nearest pickables. Will tick current PickableObject regardless of true/false
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	bool bUpdatePickableObject;
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	bool bCanEverBeActive;

	UFUNCTION() void GrabSphere_BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void GrabSphere_EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY() TArray<AActor*> OverlappedActorsArray;

private:
	UPROPERTY()
	TScriptInterface<IPlayerPickable> NearestPickableObject;
	UPROPERTY()
	TScriptInterface<IPlayerPickable> PickedObject;
	class AVRControllerBase* OwningHandController;
};
