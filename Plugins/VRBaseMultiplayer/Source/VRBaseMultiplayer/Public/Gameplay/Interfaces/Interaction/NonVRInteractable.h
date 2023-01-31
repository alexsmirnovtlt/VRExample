// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NonVRInteractable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UNonVRInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VRBASEMULTIPLAYER_API INonVRInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | NonVRPlayerInteractable")
	void NonVR_InteractionStarted(ANonVRCharacterBase* Char, const FHitResult& HitResult);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | NonVRPlayerInteractable")
	void NonVR_InteractionEnded(ANonVRCharacterBase* Char);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | NonVRPlayerInteractable")
	void NonVR_InteractionTick(ANonVRCharacterBase* Char, const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | NonVRPlayerInteractable")
	bool NonVR_IsUI() const;
	virtual bool NonVR_IsUI_Implementation() const { return false; };

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | NonVRPlayerInteractable")
	float NonVR_GetInteractionDistance() const;
	virtual float NonVR_GetInteractionDistance_Implementation() const { return 0.f; };
};
