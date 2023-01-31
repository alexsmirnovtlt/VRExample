// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "General/Interfaces/PlayerInteractable.h"

#include "WidgetActor.generated.h"

class UVRControllerState;
class AVRControllerBase;
class UNiagaraComponent;
class UWidgetComponent;

/**
 *  Base for BP Widget Actors with some convenience functions and interface to handle VR player interaction
 */
UCLASS(Abstract)
class VRBASE_API AWidgetActor : public AActor, public IPlayerInteractable
{
	GENERATED_BODY()
	
protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public: // IPlayerInteractable Interface
	virtual bool IsUI_Implementation() const override { return true; };
	
	// Use manually to force InvalidateInteractableObject() on a Hand, so it triggers EndUITrace() here for that hand
	// Gets called automatically at EndPlay for ActiveHand. 
	UFUNCTION(BlueprintCallable, Category = "VR Base | Interaction | VR Widget Actor")
	void ResetInteraction(AVRControllerBase* Hand);

	UFUNCTION(BlueprintCallable, Category = "VR Base | Interaction | VR Widget Actor")
	void ClearSlateWidgetReferenceForHand(AVRControllerBase* Hand);

	// Manually update NiagraPS data. By default NiagraPS and its parameter are set at StartUITrace() only
	UFUNCTION(BlueprintCallable, Category = "VR Base | Interaction | VR Widget Actor")
	void CacheNiagraData(AVRControllerBase* Hand);

public:
	// Optional function, UpdateUITrace calls it if needed anyway
	UFUNCTION(BlueprintCallable, Category = "VR Base | Interaction | VR Widget Actor")
	void StartUITrace(AVRControllerBase* Hand, const FHitResult& HitResult);
	UFUNCTION(BlueprintCallable, Category = "VR Base | Interaction | VR Widget Actor")
	void UpdateUITrace(AVRControllerBase* Hand, const FHitResult& HitResult);
	UFUNCTION(BlueprintCallable, Category = "VR Base | Interaction | VR Widget Actor")
	void EndUITrace(AVRControllerBase* Hand);

	// Use USimpleDataReceiver to try to pass an int value to a UserWidget
	UFUNCTION(BlueprintCallable, Category = "VR Base | Interaction | VR Widget Actor")
	void SendIntegerToAWidget(const UWidgetComponent* WidgetComponent, int32 Value);
	// Use USimpleDataReceiver to try to pass an Array to a UserWidget
	UFUNCTION(BlueprintCallable, Category = "VR Base | Interaction | VR Widget Actor")
	void SendObjectsToAWidget(const UWidgetComponent* WidgetComponent, const TArray<UObject*>& DataArray);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Customizable")
	TSubclassOf<UVRControllerState> InteractionStateClass;

	UPROPERTY(Transient, BlueprintReadWrite)
	AVRControllerBase* ActiveHand;
	UPROPERTY(Transient, BlueprintReadWrite)
	UNiagaraComponent* CachedNiagraComponent; // TODO may be reworked into TWeakObjectPtr is needed?
	UPROPERTY(Transient, BlueprintReadWrite)
	FName CachedNiagraParameterName;
};
