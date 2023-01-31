// 2021-2022 Alexander Smirnov

#pragma once

#include "General/Gameplay/Objects/LogicStates/Teleport/VRTeleportLogicBase.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "VRTeleportLogic_Arc.generated.h"

struct FPredictProjectilePathParams;

/**
 * Handles teleport and returns teleport destination if available.
 * Visualises teleport trace as an Arc
 */
UCLASS()
class VRBASE_API UVRTeleportLogic_Arc : public UVRTeleportLogicBase
{
	GENERATED_BODY()

public:
	virtual void Activate(UVRControllerState* OwningControllerState, UNiagaraComponent* NewTeleportTraceComponent, FName NiagraVectorTraceParameter) override;
	virtual void UpdateTeleportArc(float HorizontalInput, float VerticalInput, FVector StartLocation, FRotator StartRotation) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Trace Params")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceAgainstObjectTypes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace Params")
	float TeleportProjectileRadius = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace Params")
	float TeleportProjectileSpeed = 1400.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace Params")
	float TeleportMaxSimulationTime = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace Params")
	float TeleportMaxSimulationFrequency = 15.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace Params")
	bool TeleportTraceComplex = false;
	UPROPERTY(EditDefaultsOnly, Category = "Trace Params")
	FVector NavMeshCheckExtent = FVector(33.f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace Params")
	FVector TeleportDestinationLocationOffset = FVector(0.f, 0.f, -8.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace Params")
	bool bShowDebugTrace = false;

	virtual void VisualizeTeleportTrace(const FHitResult& HitInfo, const TArray<FVector> TraceLocations) override;

	TArray<AActor*> ActorsToIgnoreWhileTrace;
	UNavigationSystemV1* NavigationSystem;

	FPredictProjectilePathParams ProjectileParams;
};
