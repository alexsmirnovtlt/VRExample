// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "VRBaseMovementComponent.generated.h"

#pragma region New_child_network_classes 

class FSavedMove_CustomVRCharacter : public FSavedMove_Character // FSavedMove_VRCharacter and FSavedMove_VRBaseCharacter are already used in VRExpansion plugin
{
	typedef FSavedMove_Character Super;

public:
	FVector2D HMDLocationDelta;
	uint8 SavedRequestToStartSprinting : 1;

	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;
	virtual void SetInitialPosition(ACharacter* C) override;
	// TODO Maybe there is some cases when movement can be combined? Now it always returns false
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
};

class FNetworkPredictionData_Client_VRBaseCharacter : public FNetworkPredictionData_Client_Character
{
public:

	FNetworkPredictionData_Client_VRBaseCharacter(const UCharacterMovementComponent& ClientMovement) : FNetworkPredictionData_Client_Character(ClientMovement) {};
	virtual FSavedMovePtr AllocateNewMove() override { return FSavedMovePtr(new FSavedMove_CustomVRCharacter()); };
};

class FVRBaseCharacterNetworkMoveData : public FCharacterNetworkMoveData
{
	typedef FCharacterNetworkMoveData Super;

public:
	FVector2D HMDDelta;

	virtual void ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType) override;
	virtual bool Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType) override;
};

struct FVRBaseCharacterNetworkMoveDataContainer : public FCharacterNetworkMoveDataContainer
{
	FVRBaseCharacterNetworkMoveDataContainer()
	{
		NewMoveData = &VRBaseCharacterNetworkMoveData[0];
		PendingMoveData = &VRBaseCharacterNetworkMoveData[1];
		OldMoveData = &VRBaseCharacterNetworkMoveData[2];
	}
	virtual ~FVRBaseCharacterNetworkMoveDataContainer() {}

	FVRBaseCharacterNetworkMoveData VRBaseCharacterNetworkMoveData[3];
};

# pragma endregion

class AVRPawnBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVRPawnTeleportDelegate);

/**
 * Mostly a UCharacterMovementComponent but more suited for VR use: can move (actually sweep for short distances) without input.
 * Can jump, crouch(wip) and also sprint. Multiplayer compatible.
 * IMPORTART ROTATION NOTICE It is assumed that Rotation of all non local pawns will always be unchanged for smooth snap and mesh updates.
 *							 Local pawn updates its rotation with SnapTurn() or LocalPlayer_FaceYawRotation()
 */

UCLASS()
class VRBASE_API UVRBaseMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UVRBaseMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Pawn | VR Movement Component")
	void PerformSnapTurn(bool bToTheRight = true);
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Pawn | VR Movement Component")
	void PerformSnapTurn_OppositeRotation();
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Pawn | VR Movement Component")
	void PerformTeleport(const FVector& NewLocation, const FRotator& NewRotation, bool bAddCapsuleHalfHeight);

	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Pawn | VR Movement Component")
	void StartSprinting() { if(!IsCrouching()) RequestToStartSprinting = true; };
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Pawn | VR Movement Component")
	void StopSprinting() { RequestToStartSprinting = false; };

	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Pawn | VR Movement Component")
	void LocalPlayer_FaceYawRotation(float WorldYaw);

	void SetCanMove(bool CanMove, bool CanTurn) { bCanMove = CanMove; bCanTurn = CanTurn; } // TODO add SetMovementEnabled and SetSnapTurnEnabled so SetCanMove wont reenable movement that was explicitly disabled
	void GetCanMove(bool& CanMove, bool& CanTurn) const { CanMove = bCanMove; CanTurn = bCanTurn; }

	FVector GetHMDWorldDeltaForLastFrame();
	virtual FVector2D GetHMDWorldDeltaForReplication() { return FVector2D(GetHMDWorldDeltaForLastFrame()); }; // Will be overridden in UVRMovableCamMovementComponent
	
	UFUNCTION(BlueprintCallable, Category = "VR Base | VR Pawn | VR Movement Component")
	virtual void SetCameraDetachedState(bool bDetached) {};

	uint8 RequestToStartSprinting : 1;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Snap Turn")
	float SnapTurnYawAmount;

	UPROPERTY(BlueprintReadOnly)
	bool bCanMove;
	UPROPERTY(BlueprintReadOnly)
	bool bCanTurn;

	AVRPawnBase* GetVRBasePawn();
	AVRPawnBase* VRBasePawn;

	UPROPERTY(BlueprintAssignable, Category = "VR Base | VR Pawn | VR Movement Component")
	FVRPawnTeleportDelegate PawnTeleported;
	UPROPERTY(BlueprintAssignable, Category = "VR Base | VR Pawn | VR Movement Component")
	FVRPawnTeleportDelegate PawnSnapTurned;

	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxSprintSpeed;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "VR Base | VR Pawn | VR Movement Component")
	void Server_TeleportClient(FVector_NetQuantize100 NewLocation, FRotator NewRotation, bool bAddCapsuleHalfHeight);
	void Server_TeleportClient_Implementation(FVector_NetQuantize100 NewLocation, FRotator NewRotation, bool bAddCapsuleHalfHeight)
	{
		TeleportLocally(NewLocation, NewRotation, bAddCapsuleHalfHeight);
		Client_NotifyOfTeleport(NewLocation, NewRotation, bAddCapsuleHalfHeight);
	};
	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "VR Base | VR Pawn | VR Movement Component")
	void Client_NotifyOfTeleport(FVector_NetQuantize100 NewLocation, FRotator NewRotation, bool bAddCapsuleHalfHeight);
	void Client_NotifyOfTeleport_Implementation(FVector_NetQuantize100 NewLocation, FRotator NewRotation, bool bAddCapsuleHalfHeight)
	{
		TeleportLocally(NewLocation, NewRotation, bAddCapsuleHalfHeight);
	};
	void TeleportLocally(FVector NewLocation, FRotator NewRotation, bool bAddCapsuleHalfHeight);

#pragma region Parent_Overrides

public:
	virtual float GetMaxSpeed() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void ServerMove_PerformMovement(const FCharacterNetworkMoveData& MoveData) override;
protected:
	virtual bool ClientUpdatePositionAfterServerUpdate() override;

#pragma endregion
	
#pragma region INetworkPredictionInterface

public:
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
private:
	FVRBaseCharacterNetworkMoveDataContainer VRNetworkMoveDataContainer;

# pragma endregion

};
