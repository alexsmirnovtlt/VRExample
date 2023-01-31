// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/StreamableManager.h"
#include "General/Structs/MultiplayerStructs.h"
#include "FullBodyAvatar.generated.h"

class AVRFullBodyPlayerController;
class UVRFullBodyDataSaveGame;
class UAvatarSkeletonData;
class UAvatarDataAsset;
class AVRFullBodyPawn;
class USkeletalMesh;

/**
 * Visual representation of player`s pawn in a form of a Static Mesh with a custom AnimInstance
 */
UCLASS(BlueprintType, Blueprintable)
class VRFULLBODY_API AFullBodyAvatar : public AActor
{
	GENERATED_BODY()

	friend class AVRFullBodyPlayerController;
	friend class UVRAvatarAnimInstance;

public:
	AFullBodyAvatar();

protected:
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Immediately change Avatar based on asset
	UFUNCTION(BlueprintCallable)
	void HotSwapAvatar(UPARAM(Ref) const FPrimaryAssetId& Asset);
	void SetupAvatar(AVRFullBodyPawn* Pawn = nullptr);

	UFUNCTION(BlueprintCallable)
	bool ToggleCalibrationState() { bIsInCalibrationState = !bIsInCalibrationState; return bIsInCalibrationState; };
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AVRFullBodyPawn* GetFullBodyPawn() const { return FullbodyPawn; }

protected:
	// CM, change avatar location only if pawn moved this much away
	UPROPERTY(EditDefaultsOnly, Category = "Customizable - Movement")
	float DistanceToMoveThreshold;
	// Degrees, rotate avatar only if difference between itself and the pawn is more that this
	UPROPERTY(EditDefaultsOnly, Category = "Customizable - Movement")
	float RotationThreshold;
	// CM, if pawn is that far away, just teleport to it
	UPROPERTY(EditDefaultsOnly, Category = "Customizable - Movement")
	float TeleportThreshold;
	// CM per second, speed that avatar will be moving with for the new pawn location
	UPROPERTY(EditDefaultsOnly, Category = "Customizable - Movement")
	float MatchPawnLocationSpeed;
	// CM Squared threshold to stop when moving and comparing desired pawn location vs avatar`s current location
	UPROPERTY(EditDefaultsOnly, Category = "Customizable - Movement")
	float MatchPawnLocationStopTolerance;
	UPROPERTY(EditDefaultsOnly, Category = "Customizable - Movement")
	float MatchPawnRotationSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Customizable - Movement")
	float MatchPawnRotationStopTolerance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* AvatarRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* AvatarMesh;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ReceiveAvatarData)
	FVRFullBodyAvatarData AvatarData;
	UPROPERTY(ReplicatedUsing=OnRep_ReceiveFullBodyPawn)
	AVRFullBodyPawn* FullbodyPawn;
	UFUNCTION()
	void OnRep_ReceiveAvatarData();
	UFUNCTION()
	void OnRep_ReceiveFullBodyPawn();

	TSharedPtr<FStreamableHandle> AvatarAssetStreamableHandle;
	void OnAvatarAssetLoaded();

	UPROPERTY(BlueprintReadOnly)
	FPrimaryAssetId LastPrimaryAssetID;
	UPROPERTY(BlueprintReadOnly)
	UAvatarDataAsset* AvatarAssetData;

	void ApplyDataFromAvatarAsset(UAvatarDataAsset* AssetData);
	UFUNCTION(BlueprintImplementableEvent) void OnAvatarChanged();

	void SetupAffectorsForLocalPlayer(float AvatarScale);
	virtual void SetupAffectorsForLocalPlayer_Internal();

	virtual void UpdateActorLocation(float DeltaTime);
	virtual void UpdateActorRotation(float DeltaTime);

	bool bIsMovingTowardsPawn;
	bool bIsRotatingTowardsCamera;
	bool bRotatingTowardsCamera_Clockwise;
	FVector LastPawnLocation;
	FRotator TargetPawnRotation;

	FORCEINLINE FVector GetDesiredWorldLocation();

	bool bHasValidAvatarData;
	bool bWasSetup;

	UPROPERTY(BlueprintReadOnly) bool bIsInCalibrationState;

	bool bAlignAvatarRotationWithPawnThisFrame; // Should mostly be used in case of teleport
};
