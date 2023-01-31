// 2021-2022 Alexander Smirnov


#include "General/Gameplay/Movement/ActorComponents/VRBaseMovementComponent.h"

#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/WorldSettings.h"

#include "General/Gameplay/Movement/ActorComponents/VRCameraComponent.h"
#include "General/Gameplay/Pawns/VRPawnBase.h"

UVRBaseMovementComponent::UVRBaseMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetNetworkMoveDataContainer(VRNetworkMoveDataContainer);

	SnapTurnYawAmount = 30.f;

	MaxStepHeight = 15.f;
	MaxWalkSpeed = 200.0f;
	MaxSprintSpeed = 400.f;
	MaxWalkSpeedCrouched = 150.f;
	
	bCanWalkOffLedgesWhenCrouching = true;
	NavAgentProps.bCanCrouch = true;

	bCanMove = true;
	bCanTurn = true;
	RequestToStartSprinting = false;
}

void UVRBaseMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (GetPawnOwner()->IsLocallyControlled())
	{
		FVector WorldHMDDelta = GetHMDWorldDeltaForLastFrame();
		GetPawnOwner()->SetActorLocation(GetActorLocation() + WorldHMDDelta, true);
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UVRBaseMovementComponent::PerformSnapTurn(bool bToTheRight)
{
	if (!GetVRBasePawn() || !bCanTurn || VRBasePawn->IsCameraDetached()) return;
	ensure(VRBasePawn->IsLocallyControlled());

	VRBasePawn->AddActorWorldRotation(FRotator(0, bToTheRight ? SnapTurnYawAmount : -SnapTurnYawAmount, 0));
	PawnSnapTurned.Broadcast();
}

void UVRBaseMovementComponent::PerformSnapTurn_OppositeRotation()
{
	if (!GetVRBasePawn() || !bCanTurn || VRBasePawn->IsCameraDetached()) return;
	ensure(VRBasePawn->IsLocallyControlled());

	VRBasePawn->AddActorWorldRotation(FRotator(0, 180.f, 0));
	PawnSnapTurned.Broadcast();
}

void UVRBaseMovementComponent::PerformTeleport(const FVector& NewLocation, const FRotator& NewRotation, bool bAddCapsuleHalfHeight)
{
	if (!GetVRBasePawn() || !bCanMove) return;
	
	if(GetNetMode() == ENetMode::NM_Client) Server_TeleportClient(NewLocation, NewRotation, bAddCapsuleHalfHeight);
	else TeleportLocally(NewLocation, NewRotation, bAddCapsuleHalfHeight);
}

void UVRBaseMovementComponent::TeleportLocally(FVector NewLocation, FRotator NewRotation, bool bAddCapsuleHalfHeight)
{
	if (!GetVRBasePawn()) return;
	FVector CapsuleHalfHeight = bAddCapsuleHalfHeight ? FVector(0.f, 0.f, VRBasePawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) : FVector(0.f);
	VRBasePawn->TeleportTo(NewLocation + CapsuleHalfHeight, VRBasePawn->GetActorRotation());
	PawnTeleported.Broadcast();

	if (VRBasePawn->IsLocallyControlled())
	{
		if (VRBasePawn->IsCameraDetached()) SetCameraDetachedState(false);
		LocalPlayer_FaceYawRotation(NewRotation.Yaw);
	}
}

void UVRBaseMovementComponent::LocalPlayer_FaceYawRotation(float WorldYaw)
{
	if (!VRBasePawn || !VRBasePawn->IsLocallyControlled()) return;
	VRBasePawn->AddActorWorldRotation(FRotator(0, WorldYaw - VRBasePawn->GetCamera()->GetComponentRotation().Yaw, 0));
}

AVRPawnBase* UVRBaseMovementComponent::GetVRBasePawn()
{
	if (IsValid(VRBasePawn)) return VRBasePawn;
	if (auto Pawn = GetPawnOwner())
	{
		VRBasePawn = Cast<AVRPawnBase>(Pawn);
		return VRBasePawn;
	} else return nullptr;
}

FVector UVRBaseMovementComponent::GetHMDWorldDeltaForLastFrame()
{
	return GetPawnOwner()->GetActorRotation().RotateVector(GetVRBasePawn()->GetCamera()->GetLastHMDMovementDelta());
}

#pragma region Parent_Overrides

float UVRBaseMovementComponent::GetMaxSpeed() const
{
	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
	case MOVE_Falling:
		if (IsCrouching()) return MaxWalkSpeedCrouched;
		else return RequestToStartSprinting ? MaxSprintSpeed : MaxWalkSpeed;
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		return MaxCustomMovementSpeed;
	case MOVE_None:
	default:
		return 0.f;
	}
}

void UVRBaseMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	RequestToStartSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

void UVRBaseMovementComponent::ServerMove_PerformMovement(const FCharacterNetworkMoveData& MoveData)
{
	// A copy of parent function, except sweep with HMDData before actual move

	if (!HasValidData() || !IsActive())
	{
		return;
	}

	const float ClientTimeStamp = MoveData.TimeStamp;
	FVector_NetQuantize10 ClientAccel = MoveData.Acceleration;
	const uint8 ClientMoveFlags = MoveData.CompressedMoveFlags;
	const FRotator ClientControlRotation = MoveData.ControlRotation;

	FNetworkPredictionData_Server_Character* ServerData = GetPredictionData_Server_Character();
	check(ServerData);

	if (!VerifyClientTimeStamp(ClientTimeStamp, *ServerData))
	{
		const float ServerTimeStamp = ServerData->CurrentClientTimeStamp;
		return;
	}

	bool bServerReadyForClient = true;
	APlayerController* PC = Cast<APlayerController>(CharacterOwner->GetController());
	if (PC)
	{
		bServerReadyForClient = PC->NotifyServerReceivedClientData(CharacterOwner, ClientTimeStamp);
		if (!bServerReadyForClient)
		{
			ClientAccel = FVector::ZeroVector;
		}
	}

	const UWorld* MyWorld = GetWorld();
	const float DeltaTime = ServerData->GetServerMoveDeltaTime(ClientTimeStamp, CharacterOwner->GetActorTimeDilation(*MyWorld));

	if (DeltaTime > 0.f)
	{
		ServerData->CurrentClientTimeStamp = ClientTimeStamp;
		ServerData->ServerAccumulatedClientTimeStamp += DeltaTime;
		ServerData->ServerTimeStamp = MyWorld->GetTimeSeconds();
		ServerData->ServerTimeStampLastServerMove = ServerData->ServerTimeStamp;

		if (PC)
		{
			PC->SetControlRotation(ClientControlRotation);
		}

		if (!bServerReadyForClient)
		{
			return;
		}

		if ((MyWorld->GetWorldSettings()->GetPauserPlayerState() == NULL))
		{
			if (PC)
			{
				PC->UpdateRotation(DeltaTime);
			}

			// New lines. Sweep for hmd movement before actual move, as the local player did
			const auto VRBaseMoveData = static_cast<const FVRBaseCharacterNetworkMoveData&>(MoveData);

			auto SweepLocation = CharacterOwner->GetActorLocation() + FVector(VRBaseMoveData.HMDDelta.X, VRBaseMoveData.HMDDelta.Y, 0.f);
			CharacterOwner->SetActorLocation(SweepLocation, true);
			//

			MoveAutonomous(ClientTimeStamp, DeltaTime, ClientMoveFlags, ClientAccel);
		}

		UE_CLOG(CharacterOwner && UpdatedComponent, LogNetPlayerMovement, VeryVerbose, TEXT("ServerMove Time %f Acceleration %s Velocity %s Position %s Rotation %s DeltaTime %f Mode %s MovementBase %s.%s (Dynamic:%d)"),
			ClientTimeStamp, *ClientAccel.ToString(), *Velocity.ToString(), *UpdatedComponent->GetComponentLocation().ToString(), *UpdatedComponent->GetComponentRotation().ToCompactString(), DeltaTime, *GetMovementName(),
			*GetNameSafe(GetMovementBase()), *CharacterOwner->GetBasedMovement().BoneName.ToString(), MovementBaseUtility::IsDynamicBase(GetMovementBase()) ? 1 : 0);
	}

	if (MoveData.NetworkMoveType == FCharacterNetworkMoveData::ENetworkMoveType::NewMove)
	{
		ServerMoveHandleClientError(ClientTimeStamp, DeltaTime, ClientAccel, MoveData.Location, MoveData.MovementBase, MoveData.MovementBaseBoneName, MoveData.MovementMode);
	}
}

bool UVRBaseMovementComponent::ClientUpdatePositionAfterServerUpdate()
{
	UE_LOG(VRBaseLog, Log, TEXT("%s"), *FString(__FUNCTION__));
	// TODO Probably reset detached camera offset and reattach here
	return Super::ClientUpdatePositionAfterServerUpdate();
}

FNetworkPredictionData_Client* UVRBaseMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UVRBaseMovementComponent* MutableThis = const_cast<UVRBaseMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_VRBaseCharacter(*this);
	}

	return ClientPredictionData;
}

#pragma endregion

#pragma region Additional_network_movement_classes

void FSavedMove_CustomVRCharacter::Clear()
{
	Super::Clear();
	SavedRequestToStartSprinting = 0;
}

uint8 FSavedMove_CustomVRCharacter::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	if (SavedRequestToStartSprinting) Result |= FLAG_Custom_0;
	return Result;
}

void FSavedMove_CustomVRCharacter::SetInitialPosition(ACharacter* C)
{
	Super::SetInitialPosition(C);

	AVRPawnBase* VRPawn = static_cast<AVRPawnBase*>(C);
	HMDLocationDelta = VRPawn->GetVRMovementComponent()->GetHMDWorldDeltaForReplication();
	SavedRequestToStartSprinting = VRPawn->GetVRMovementComponent()->RequestToStartSprinting;
}

bool FSavedMove_CustomVRCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	// Local player`s HMD Movement delta for every tick will always be != Zero so moves should not be combined ever. Slight bandwidth increase
	return false;
	// if (SavedRequestToStartSprinting != ((FSavedMove_CustomVRCharacter*)&NewMove)->SavedRequestToStartSprinting) return false;
	// else return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void FVRBaseCharacterNetworkMoveData::ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType)
{
	Super::ClientFillNetworkMoveData(ClientMove, MoveType);

	const auto CustomSavedMove = static_cast<const FSavedMove_CustomVRCharacter&>(ClientMove);
	HMDDelta = CustomSavedMove.HMDLocationDelta;
}

bool FVRBaseCharacterNetworkMoveData::Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType)
{
	// An exact copy of parent, one new line

	NetworkMoveType = MoveType;

	bool bLocalSuccess = true;
	const bool bIsSaving = Ar.IsSaving();

	Ar << TimeStamp;

	Acceleration.NetSerialize(Ar, PackageMap, bLocalSuccess);
	Location.NetSerialize(Ar, PackageMap, bLocalSuccess);
	
	HMDDelta.NetSerialize(Ar, PackageMap, bLocalSuccess); // that one

	ControlRotation.NetSerialize(Ar, PackageMap, bLocalSuccess);

	SerializeOptionalValue<uint8>(bIsSaving, Ar, CompressedMoveFlags, 0);

	if (MoveType == ENetworkMoveType::NewMove)
	{
		SerializeOptionalValue<UPrimitiveComponent*>(bIsSaving, Ar, MovementBase, nullptr);
		SerializeOptionalValue<FName>(bIsSaving, Ar, MovementBaseBoneName, NAME_None);
		SerializeOptionalValue<uint8>(bIsSaving, Ar, MovementMode, MOVE_Walking);
	}

	return !Ar.IsError();
}

#pragma endregion