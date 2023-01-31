// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Actors/FullBodyAvatar.h"
#include "VRTrackersFullBodyAvatar.generated.h"

/**
 * 
 */
UCLASS()
class VRFULLBODY_API AVRTrackersFullBodyAvatar : public AFullBodyAvatar
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void SetupAffectorsForLocalPlayer_Internal() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trackers FullBody Avatar")
	bool bUpdateAvatarTransform = true;

	virtual void UpdateActorLocation(float DeltaTime) override;
	virtual void UpdateActorRotation(float DeltaTime) override;
};
