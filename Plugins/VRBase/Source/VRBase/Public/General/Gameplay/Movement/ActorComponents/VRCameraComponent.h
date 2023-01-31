// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "VRCameraComponent.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class VRBASE_API UVRCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	UVRCameraComponent();
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	FORCEINLINE FVector GetLastHMDLocation() const { return LastHMDLocation; };
	FORCEINLINE FVector GetLastHMDMovementDelta() const { return FVector(LastHMDMovementDelta, 0.f); };

protected:
	UPROPERTY(BlueprintReadOnly)
	FVector LastHMDLocation;
	UPROPERTY(BlueprintReadOnly)
	FVector2D LastHMDMovementDelta;

	bool bDropFirstHMDDelta; // To prevent possible big initial delta when user starts app while not in the center or their guardian space 
};
