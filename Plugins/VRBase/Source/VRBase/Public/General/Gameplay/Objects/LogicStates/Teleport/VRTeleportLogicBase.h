// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VRTeleportLogicBase.generated.h"

class UNavigationSystemV1;
class UVRControllerState;
class UNiagaraComponent;
class AActor;

struct FStreamableHandle;

/**
 * Handles teleport and returns teleport destination if available.
 * Overridable base class with empty logic.
 * Teleport destination`s  rotation is the same as Player Camera
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class VRBASE_API UVRTeleportLogicBase : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	virtual void Activate(UVRControllerState* OwningControllerState, UNiagaraComponent* NewTeleportTraceComponent, FName NiagraVectorTraceParameter);
	UFUNCTION(BlueprintCallable)
	virtual void Deactivate();

	UFUNCTION(BlueprintCallable)
	virtual void UpdateTeleportArc(float HorizontalInput, float VerticalInput, FVector StartLocation, FRotator StartRotation) {};
	UFUNCTION(BlueprintCallable)
	virtual void GetTeleportDestination(bool& bIsValidLocation, FVector& Location, FRotator& Rotation);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Assignable")
	TSoftClassPtr<AActor> TeleportDestinationAsset;

	virtual void VisualizeTeleportTrace(const FHitResult& HitInfo, const TArray<FVector> TraceLocations) {};
	void OnTeleportDestinationLoaded();

	UPROPERTY()
	AActor* TeleportDestinationActor;
	TSharedPtr<FStreamableHandle> TeleportDestinationHandle;

	UNiagaraComponent* TeleportTraceComponent;
	FName NiagraVectorArrayParameter = FName("");

	UVRControllerState* ControllerState;

	//FVector LastValidTeleportLocation; // TODO Just for about a second if Target Actor is not yet loaded, player cant teleport even if valid locatation is found
};
