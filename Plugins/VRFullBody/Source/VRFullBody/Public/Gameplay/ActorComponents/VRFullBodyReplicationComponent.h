// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "General/Gameplay/Networking/ActorComponents/VRBaseDataReplicationComponent.h"
#include "VRFullBodyReplicationComponent.generated.h"

class AVRFullBodyPawn;

/**
 * 
 */
UCLASS()
class VRFULLBODY_API UVRFullBodyReplicationComponent : public UVRBaseDataReplicationComponent
{
	GENERATED_BODY()

public:
	virtual bool GetLocalDataToReplicate(float DeltaTime, FVRBasePlayerData& DataRef) override;

protected:
	AVRFullBodyPawn* GetVRFullBodyPawn();
	AVRFullBodyPawn* OwnerVRFullBodyPawn;

	FORCEINLINE void CombineRelative(const USceneComponent* Parent, const USceneComponent* Child, FVector& OutLoc, FRotator& OutRot)
	{
		FTransform NewTransform = Child->GetRelativeTransform() * Parent->GetRelativeTransform();
		OutLoc = NewTransform.GetLocation();
		OutRot = NewTransform.GetRotation().Rotator();
	}
};
