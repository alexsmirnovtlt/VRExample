#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInteractable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UPlayerInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Should mostly be used for player interaction with objects from a distance (UI or some actor)
 */
class VRBASE_API IPlayerInteractable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | PlayerInteractable")
	void InteractionStarted(AVRControllerBase* Hand, const FHitResult& HitResult);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | PlayerInteractable")
	void InteractionEnded(AVRControllerBase* Hand);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | PlayerInteractable")
	void InteractionTick(AVRControllerBase* Hand, const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | PlayerInteractable")
	bool IsUI() const;
	virtual bool IsUI_Implementation() const { return false; };

	// Distance for Actor to be considered interactable.
	// -1: completely non interactable. 0: unbound distance. >0: in cm
	// Would not exceed Hand`s max distance
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | PlayerInteractable")
	float GetInteractionDistance(AVRControllerBase* Hand) const;
	virtual float GetInteractionDistance_Implementation(AVRControllerBase* Hand) const { return 0.f; };
};
