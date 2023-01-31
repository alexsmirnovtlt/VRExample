// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "General/Interfaces/PlayerInteractable.h"

#include "DistantInteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), hidecategories = ("Variable", "Tags", "ComponentTick", "ComponentReplication", "Activation", "Cooking", "AssetUserData", "Collision", "Sockets", "Events"))
class VRBASE_API UDistantInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDistantInteractionComponent();

public:
	void SetupComponent(class AVRControllerBase* OwningHand, class  UWidgetInteractionComponent* WidgetInteractionComp);
	virtual void UpdateInteraction(float DeltaTime, bool bTickOnly);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	// Forcefully resets InteractableObject and calls OnInteractionEnd on it.
	// Useful when InteractableObject must be destroyed but want to handle OnInteractionEnd()
	UFUNCTION(BlueprintCallable, Category = "VR Base | Controller | Interaction")
	void InvalidateInteractableObject();
	UFUNCTION(BlueprintCallable, Category = "VR Base | Controller | Interaction")
	void ForceUpdateInteractableObject(const TScriptInterface<IPlayerInteractable>& NewInteractableObject, bool InvalidatePrevious);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VR Base | Controller | Interaction")
	const TScriptInterface<IPlayerInteractable>& GetInteractableObject() const { return InteractableObject; };

	UFUNCTION(BlueprintCallable, Category = "VR Base | Controller | Interaction")
	void SetKeepUpdatingInteraction(bool bActive, bool bForceStateChange = false);

	UFUNCTION(BlueprintCallable, Category = "VR Base | Controller | Interaction")
	void ChangeInteractableObjectCheckDistance(float NewDistance) { InteractableObjectsCheckDistance = NewDistance; };

	UFUNCTION(BlueprintCallable, Category = "VR Base | Controller | Interaction")
	void ClearSlateWidgetReference();

protected:
	// If true will trigger IPlayerInteractable interface calls on active interactables. Will tick current InteractableObject regardless of true/false
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	bool bUpdateInteractableObject;
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	bool bCanEverBeActive;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractableObjectsCheckDistance = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> HandInteractableObjectType;

private:
	UPROPERTY()
	TScriptInterface<IPlayerInteractable> InteractableObject; // TScriptInterface Is VERY unreliable in pure C++, should only be used in BPs! Cast<> to interface will ALWAYS return nullptr in C++ if interface was added in BP
	class AVRControllerBase* OwningHandController;
	class UWidgetInteractionComponent* WidgetInteraction;
};
