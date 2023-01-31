// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerPickable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UPlayerPickable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for any actor that can be picked up by the VR player
 */
class VRBASE_API IPlayerPickable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | PlayerPickable")
	void OnHandProximity_Start(AVRControllerBase* HandController, USceneComponent* CollidedComponent);
	virtual void OnHandProximity_Start_Implementation(AVRControllerBase* HandController, USceneComponent* CollidedComponent) {};
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | PlayerPickable")
	void OnHandProximity_End(AVRControllerBase* HandController, USceneComponent* CollidedComponent);
	virtual void OnHandProximity_End_Implementation(AVRControllerBase* HandController, USceneComponent* CollidedComponent) {};

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | PlayerPickable")
	void OnGrab(AVRControllerBase* HandController);
	virtual void OnGrab_Implementation(AVRControllerBase* HandController) {};
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | PlayerPickable")
	void OnDrop(AVRControllerBase* HandController);
	virtual void OnDrop_Implementation(AVRControllerBase* HandController) {};

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | PlayerPickable")
	void OnHandControllerTick(AVRControllerBase* HandController);
	virtual void OnHandControllerTick_Implementation(AVRControllerBase* HandController) {};

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | PlayerPickable")
	bool IsGrabDisabled(AVRControllerBase* HandController);
	virtual bool IsGrabDisabled_Implementation(AVRControllerBase* HandController) const { return false; };
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | PlayerPickable")
	bool IsDropDisabled(AVRControllerBase* HandController);
	virtual bool IsDropDisabled_Implementation(AVRControllerBase* HandController) const { return false; };

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | PlayerPickable")
	bool IsGrabbed();
	virtual bool IsGrabbed_Implementation() const { return false; };

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | PlayerPickable")
	FVector GetWorldLocationForProximityCheck(AVRControllerBase* HandController);
	virtual FVector GetWorldLocationForProximityCheck_Implementation(AVRControllerBase* HandController) const { ensure(false); return FVector(0.f); };
};
