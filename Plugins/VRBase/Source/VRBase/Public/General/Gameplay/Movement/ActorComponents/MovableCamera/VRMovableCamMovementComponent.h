// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "General/Gameplay/Movement/ActorComponents/VRBaseMovementComponent.h"
#include "VRMovableCamMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class VRBASE_API UVRMovableCamMovementComponent : public UVRBaseMovementComponent
{
	GENERATED_BODY()
	
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	bool IsCameraDetached() const { return bDetachedCam; };
	virtual FVector2D GetHMDWorldDeltaForReplication() override;

	virtual void SetCameraDetachedState(bool bDetached) override;

protected:
	// Disable Detachable camera functionality and behave just like a parent class
	UPROPERTY(EditDefaultsOnly, Category = "Movable Camera Vars")
	bool DisableMovableCamera = false;
	UPROPERTY(EditDefaultsOnly, Category = "Movable Camera Vars")
	float MaxPawnSweepCMLengthPerFrame = 1.f;

	FVector DetachedCam_GetPawnSweepLocation(const FVector& PawnLocation, const FVector& TargetCamLocation);

	FVector2D ReplicatedHMDDelta;
	bool bDetachedCam;
};
