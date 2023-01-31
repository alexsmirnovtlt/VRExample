// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Player/Pawn/VRMultiplayerPawn.h"
#include "VRFullBodyPawn.generated.h"

class AFullBodyAvatar;
class USceneComponent;

/**
 * Base class for Full Body pawn implementation
 */
UCLASS()
class VRFULLBODY_API AVRFullBodyPawn : public AVRMultiplayerPawn
{
	GENERATED_BODY()

	friend class UVRFullBodyReplicationComponent;
	friend class AVRFullBodyPlayerController;
	friend class UVRAvatarAnimInstance;
	friend class AFullBodyAvatar;

public:
	AVRFullBodyPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void SetAvatarReferenceOnce(AFullBodyAvatar* NewPtr);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FRotator GetPlayerForwardWorldRotation() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VR Pawn Setup - Avatar")
	TSubclassOf<AFullBodyAvatar> FullBodyAvatarClass;
	// Workaround for forward direction of different skeleton root rotations or the waist bone of the trackers pawn
	UPROPERTY(BlueprintReadWrite, Category = "VR Pawn Setup - Avatar")
	float AdditionalYawAmountPerTick = 0.f;

	UPROPERTY(ReplicatedUsing=OnRep_AvatarActor, BlueprintReadOnly)
	AFullBodyAvatar* Avatar;

	EAxis::Type WaistForwardAxis; // Used on a trackers pawn (child class)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* AvatarRootGimbal;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* AvatarRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* LocalPlayerHeadBone;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* LocalPlayerBone_Hand_Right;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* LocalPlayerBone_Hand_Left;

	UFUNCTION()
	void OnRep_AvatarActor();
};
