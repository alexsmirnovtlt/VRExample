#pragma once

#include "General/Gameplay/Objects/LogicStates/Teleport/VRTeleportLogicBase.h"
#include "VRTeleportLogic_NoArc.generated.h"

struct FCollisionQueryParams;
struct FCollisionObjectQueryParams;

/**
 * Handles teleport and returns teleport destination if available.
 * Visualises teleport trace as a straight line
 */
UCLASS()
class VRBASE_API UVRTeleportLogic_NoArc : public UVRTeleportLogicBase
{
	GENERATED_BODY()
	
public:
	virtual void Activate(UVRControllerState* OwningControllerState, UNiagaraComponent* NewTeleportTraceComponent, FName NiagraVectorTraceParameter) override;
	virtual void UpdateTeleportArc(float HorizontalInput, float VerticalInput, FVector StartLocation, FRotator StartRotation) override;

protected:

	void VisualizeTeleportTrace(const FHitResult& HitInfo, const TArray<FVector> TraceLocations) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace Params")
	float TraceLength = 2000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace Params")
	bool bHideTraceOnInvalidDestination = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace Params")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceAgainstObjectTypes;
	UPROPERTY(EditDefaultsOnly, Category = "Trace Params")
	FVector NavMeshCheckExtent = FVector(33.f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace Params")
	FVector TeleportDestinationLocationOffset = FVector(0.f, 0.f, -8.f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace Params")
	bool TeleportTraceComplex = false;

	UNavigationSystemV1* NavigationSystem;

	FCollisionQueryParams QueryParams;
	FCollisionObjectQueryParams ObjectQueryParams;
};
