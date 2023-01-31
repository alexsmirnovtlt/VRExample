// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "General/Gameplay/Objects/LogicStates/Teleport/VRTeleportLogic_Arc.h"
#include "VRTeleportLogic_Arc_Rotatable.generated.h"

/**
 * 
 */
UCLASS()
class VRBASE_API UVRTeleportLogic_Arc_Rotatable : public UVRTeleportLogic_Arc
{
	GENERATED_BODY()
public:
	virtual void UpdateTeleportArc(float HorizontalInput, float VerticalInput, FVector StartLocation, FRotator StartRotation) override;
	virtual void GetTeleportDestination(bool& bIsValidLocation, FVector& Location, FRotator& Rotation);
};
