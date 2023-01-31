// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "General/Input/Interfaces/PlayerInputReceiver.h"
#include "VRControllerState.generated.h"

class AVRControllerBase;
class AActor;

/**
 * Main logic class for Motion Controllers.
 * Should handle input and switch to another ControllerState when nessessary
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class VRBASE_API UVRControllerState : public UObject, public IPlayerInputReceiver
{
	GENERATED_BODY()

public:
	UVRControllerState();
	void Setup(AVRControllerBase* NewOwningController, UClass* NewPreviousState);

	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Controller State")
	AVRControllerBase* GetVRController() const { return OwningHandController; };
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Controller State")
	AVRControllerBase* GetVRController_ForAnotherHand() const;
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Controller State")
	void BackToPreviousState(bool bNotifyOtherController);
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Controller State")
	AActor* SpawnActor(const TSubclassOf<AActor>& ClassToSpawn, ESpawnActorCollisionHandlingMethod SpawnMethod);

	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Controller State")
	bool IsDistantInteractionAllowed() const { return bUpdateDistantInteraction; }
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Controller State")
	bool IsUIInteractionAllowed() const { return bUpdateUIInteraction; }
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Controller State")
	bool IsGrabAllowed() const { return bUpdateGrab; }

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "VR Controller State")
	void Tick(float DeltaTime);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "VR Controller State")
	void OnStateEnter();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "VR Controller State")
	void OnStateExit();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "VR Controller State")
	void OnOtherControllerStateChanged(UVRControllerState* OtherState);

	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Controller State")
	const TArray<FName>& GetControllerStateTags() const { return CustomTags; };

	// Its a UObject so regular BP node is unavailable, does the same
	UFUNCTION(BlueprintPure, Category = "Utilities|Time")
	double GetWorldDeltaSeconds() const;

protected:
	// Should interact with other possible Distant Interactables
	UPROPERTY(EditDefaultsOnly, Category = "Customizable")
	bool bUpdateDistantInteraction;
	// If bUpdateDistantInteraction is true should WidgetInteraction update its hovered widget
	UPROPERTY(EditDefaultsOnly, Category = "Customizable")
	bool bUpdateUIInteraction;
	// Should interact with other possible Pickables
	UPROPERTY(EditDefaultsOnly, Category = "Customizable")
	bool bUpdateGrab;
	UPROPERTY(EditDefaultsOnly, Category = "Customizable")
	TArray<FName> CustomTags;

	UPROPERTY(BlueprintReadOnly, Category = "VR Controller State")
	AVRControllerBase* OwningHandController;
	UPROPERTY(BlueprintReadOnly, Category = "VR Controller State")
	UClass* PreviousState;
};
