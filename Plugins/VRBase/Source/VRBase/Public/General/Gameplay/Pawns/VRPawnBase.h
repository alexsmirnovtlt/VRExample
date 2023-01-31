// 2021-2023 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "General/Input/Interfaces/PlayerInputReceiver.h"
#include "General/Structs/MultiplayerStructs.h"

#include "VRPawnBase.generated.h"

class UVRBaseMovementComponent;
class UVRCameraComponent;
class AVRControllerBase;
class UCapsuleComponent;
class USceneComponent;

/**
 * Base for all VR pawn implementations. Can do the following:
 *  - Updates relative transforms of HMD (at VRCamera) and controllers (by itself)
 *  - Spawns and manages Hand (VRControllerBase) classes
 *  - Multiplayer ready, incl movement and teleport
 *	- Handles player input by forwarding input to hands
 *  - Logging for the most important logic
 */
UCLASS()
class VRBASE_API AVRPawnBase : public ACharacter, public IPlayerInputReceiver
{
	GENERATED_BODY()

public:
	AVRPawnBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void PawnClientRestart() override;
	virtual void OnRep_PlayerState() override;

public:
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Pawn")
	void SwitchVRControllers(const TSubclassOf<AVRControllerBase>& LeftHandClass, const TSubclassOf<AVRControllerBase>& RightHandClass);
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Pawn")
	void SwitchVRController(const TSubclassOf<AVRControllerBase>& HandClass, bool bIsRightHand);

	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Pawn")
	FORCEINLINE AVRControllerBase* GetHandController(bool bIsRightHand) const { return bIsRightHand ? RightHand : LeftHand; };
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Pawn")
	FORCEINLINE USceneComponent* GetGripComponent(bool bIsRightHand) const { return bIsRightHand ? GripLocation_RightHand : GripLocation_LeftHand; };
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Pawn")
	FORCEINLINE USceneComponent* GetAimComponent(bool bIsRightHand) const { return bIsRightHand ? AimLocation_RightHand : AimLocation_LeftHand; };
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Pawn")
	FORCEINLINE UVRCameraComponent* GetCamera() const { return MainCamera; };
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Pawn")
	FORCEINLINE USceneComponent* GetFloorLevelComponent() const { return FloorLevelComponent; };

	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Pawn")
	UVRBaseMovementComponent* GetVRMovementComponent();
	FORCEINLINE const UVRBaseMovementComponent* GetVRMovementComponent() const { return VRMovementComponent; };
	
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Pawn")
	virtual bool IsCameraDetached() const { return false; }; // See AVRPawnWithMovableCam for more info
	UFUNCTION(BlueprintImplementableEvent)
	void OnCameraDetachStateChanged(bool bIsDetachedNow);

	void SetSpawnControllersOnPossess(bool bSpawn) { bSpawnControllersOnPossess = bSpawn; }; // Should be set before PlayerControllerPossesses this pawn

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VR Pawn Setup - Local Player")
	bool bUpdateGripLocations;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VR Pawn Setup - Local Player")
	bool bUpdateAimLocations;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VR Pawn Setup - Local Player")
	bool bSpawnControllersOnPossess;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VR Pawn Setup - General")
	TSubclassOf<AVRControllerBase> LeftHandControllerClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VR Pawn Setup - General")
	TSubclassOf<AVRControllerBase> RightHandControllerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VR Pawn Setup - HMD - Motion Sources")
	FName Grip_Right = TEXT("RightGrip");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VR Pawn Setup - HMD - Motion Sources")
	FName Grip_Left = TEXT("LeftGrip");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VR Pawn Setup - HMD - Motion Sources")
	FName Aim_Right = TEXT("RightAim");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VR Pawn Setup - HMD - Motion Sources")
	FName Aim_Left = TEXT("LeftAim");

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* FloorLevelComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UVRCameraComponent* MainCamera;
	UPROPERTY(ReplicatedUsing = OnRep_Hand_Left, BlueprintReadOnly)
	AVRControllerBase* LeftHand;
	UPROPERTY(ReplicatedUsing = OnRep_Hand_Right, BlueprintReadOnly)
	AVRControllerBase* RightHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* GripLocation_RightHand;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* AimLocation_RightHand;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* GripLocation_LeftHand;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* AimLocation_LeftHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UVRInputBindingsActorComponent* VRInputBindingsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UVRBaseMovementComponent* VRMovementComponent;

	virtual void UpdateControllerLocation(USceneComponent* UpdatingComponent, const FVector& NewLocation, const FRotator& NewRotation);
	virtual void UpdateMotionControllersLocations(); // OpenXR specific implementation, can be overriden

	// IPlayerInputReceiver interface
public:
	virtual TScriptInterface<IPlayerInputReceiver> GetInputReceiver_Implementation(bool IsRightHand) const override;
	// ~IPlayerInputReceiver interface

	// Network support
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Can be used both on a server and clients. At this point we have a valid PlayerController for a local player. PlayerState validity at this moment is uncertain on the clients
	UFUNCTION(BlueprintImplementableEvent)
	void OnPawnReadyToPlay(bool bLocalPlayer);
	virtual void OnPawnReadyToPlay_Internal(bool bLocalPlayer);

	virtual void ReplicateVRDataToServer(float DeltaTime);

	UFUNCTION() void OnRep_Hand_Left();
	UFUNCTION() void OnRep_Hand_Right();

	UFUNCTION(Server, WithValidation, Unreliable)
	void Server_UpdateVRBaseData(const FVRBasePlayerData&  NewReplicatedBaseData);
	bool Server_UpdateVRBaseData_Validate(const FVRBasePlayerData&  NewReplicatedBaseData) { return true; }; // We actually cant really validate that data

	UPROPERTY(ReplicatedUsing=OnRep_BaseVRData)
	FVRBasePlayerData ReplicatedBaseData;
	UFUNCTION()
	void OnRep_BaseVRData();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UVRBaseDataReplicationComponent* VRBaseDataReplicationComponent;
};
