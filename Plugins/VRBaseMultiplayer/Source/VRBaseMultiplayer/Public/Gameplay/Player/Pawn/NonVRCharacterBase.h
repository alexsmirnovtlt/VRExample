// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Gameplay/Interfaces/Interaction/NonVRInteractable.h"
#include "Gameplay/Interfaces/VoipTalkerStatusReceiver.h"

#include "NonVRCharacterBase.generated.h"

class AVRMultiplayerPlayerController;

UCLASS()
class VRBASEMULTIPLAYER_API ANonVRCharacterBase : public ACharacter, public IVoipTalkerStatusReceiver
{
	GENERATED_BODY()

public:
	ANonVRCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void PawnClientRestart() override;
	virtual void OnRep_PlayerState() override;
	virtual void PossessedBy(AController* NewController) override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NonVR Base | Interaction")
	const TScriptInterface<INonVRInteractable>& GetInteractableObject() const { return InteractableObject; };
	UFUNCTION(BlueprintCallable, Category = "NonVR Base | Interaction")
	void EndInteraction();

	UFUNCTION(BlueprintCallable, Category = "NonVR Base")
	AVRMultiplayerPlayerController* GetMultiplayerPlayerController();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Simulated Proxy")
	float SimulatedProxyCameraLerpModifier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float InteractionDistance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> InteractionChannel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	bool bTraceComplex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	float MouseSensitivityScale;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pawn")
	FRotator GetSmoothedRemoteViewPitchRotation(FRotator CurrenRotation, float DeltaTime, float LerpModifier); // RemoteViewPitch is not exposed for Blueprint for some reason

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* MainCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	class UWidgetInteractionComponent* WidgetInteraction;

	UFUNCTION(BlueprintImplementableEvent)
	void OnPawnReadyToPlay(bool bLocalPlayer);
	void OnPawnReadyToPlay_Internal(bool bLocalPlayer);

	UFUNCTION(BlueprintCallable, Category = "NonVR Base")
	void LeaveOnlineSession();
	void OnOnlineSessionDestroyed(bool bSuccess);
	UFUNCTION(BlueprintImplementableEvent, Category = "NonVR Base")
	void OnOnlineSessionClosed();
	FDelegateHandle DestroySessionHandle;

	UFUNCTION(BlueprintCallable, Category = "NonVR Base")
	void CleanupVoiceData();

	void LineTraceForInteractableObjects();

	UPROPERTY()
	TScriptInterface<INonVRInteractable> InteractableObject;

	UPROPERTY(EditDefaultsOnly, Category = "VOIP")
	USoundAttenuation* VOIPAttenuation;
	UPROPERTY(VisibleAnywhere)
	UMultiplayerVOIPTalker* VOIPTalker;

private:
	void SetupHardcodedInput(UInputComponent* PlayerInputComponent);

	UFUNCTION(BlueprintCallable, Category = "NonVR Base | Interaction")
	void ClearSlateWidgetReference();

	void MoveForward(float Value) { AddMovementInput(GetActorForwardVector(), Value); };
	void MoveRight(float Value) { AddMovementInput(GetActorRightVector(), Value); };
};
