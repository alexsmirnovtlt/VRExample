// 2021-2023 Alexander Smirnov


#include "General/Gameplay/Pawns/VRPawnBase.h"

#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "IXRTrackingSystem.h"
#include "Net/UnrealNetwork.h"

#include "General/Gameplay/Networking/ActorComponents/VRBaseDataReplicationComponent.h"
#include "General/Gameplay/Movement/ActorComponents/VRBaseMovementComponent.h"
#include "General/Input/ActorComponents/VRInputBindingsActorComponent.h"
#include "General/Gameplay/Movement/ActorComponents/VRCameraComponent.h"
#include "General/Gameplay/Actors/VRControllerBase.h"
#include "General/Interfaces/Notifiable.h"

AVRPawnBase::AVRPawnBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer
		.SetDefaultSubobjectClass<UVRBaseMovementComponent>(ACharacter::CharacterMovementComponentName)
		//.DoNotCreateDefaultSubobject(ACharacter::MeshComponentName) // TODO Why it crashes UVRBaseMovementComponent::ServerMove_PerformMovement at line MoveAutonomous ?
	)
{
	bSpawnControllersOnPossess = true;
	bUpdateGripLocations = true;
	bUpdateAimLocations = true;
	
	bReplicates = false;

	bUseControllerRotationYaw = false; // Pawn rotation Yaw will be changed manually on LocallyControlled pawns only, see UVRBaseMovementComponent description and its SnapTurn logic for more info

	AutoPossessAI = EAutoPossessAI::Disabled;
	
	if (auto CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCapsuleSize(5.f, 92.f);
		CapsuleComp->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	}

	// Floor level - parent for camera and controllers
	FloorLevelComponent = CreateDefaultSubobject<USceneComponent>(TEXT("FloorLevelComponent"));
	FloorLevelComponent->SetupAttachment(RootComponent);
	FloorLevelComponent->SetRelativeLocation(FVector(0.f, 0.f, -1 * GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));

	// VR Camera
	MainCamera = CreateDefaultSubobject<UVRCameraComponent>(TEXT("CameraComponent"));
	MainCamera->SetupAttachment(FloorLevelComponent);
	MainCamera->SetRelativeLocation(FVector(0.f, 0.f, 2.f * GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));

	// Locations for Motion controllers that will be updated by the pawn itself
	GripLocation_RightHand = CreateDefaultSubobject<USceneComponent>(TEXT("GripLocation_RightHand"));
	GripLocation_RightHand->SetupAttachment(FloorLevelComponent);
	GripLocation_LeftHand = CreateDefaultSubobject<USceneComponent>(TEXT("GripLocation_LeftHand"));
	GripLocation_LeftHand->SetupAttachment(FloorLevelComponent);
	
	AimLocation_RightHand = CreateDefaultSubobject<USceneComponent>(TEXT("AimLocation_RightHand"));
	AimLocation_RightHand->SetupAttachment(FloorLevelComponent);
	AimLocation_LeftHand = CreateDefaultSubobject<USceneComponent>(TEXT("AimLocation_LeftHand"));
	AimLocation_LeftHand->SetupAttachment(FloorLevelComponent);

	// Component that handles input bindings and input callbacks to this class
	VRInputBindingsComponent = CreateDefaultSubobject<UVRInputBindingsActorComponent>(TEXT("VRInputBindings"));
	// Network replication of tracked data (HMD and controllers) 
	VRBaseDataReplicationComponent = CreateDefaultSubobject<UVRBaseDataReplicationComponent>(TEXT("VRBaseDataReplication"));
}

void AVRPawnBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(VRBaseLog, Log, TEXT("%s - LocallyControlled: %d, RighHand: %d, LeftHand: %d"), 
		*FString(__FUNCTION__),
		IsLocallyControlled() ? 1 : 0,
		RightHand ? 1 : 0,
		LeftHand ? 1 : 0);
}

void AVRPawnBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Creating input component so we dont need do bind everything right here. That object will use IPlayerInputReceiver interface to pass input back to this class
	UE_LOG(VRBaseLog, Log, TEXT("%s"), *FString(__FUNCTION__));
	if(APlayerController* PC = Cast<APlayerController>(GetController()))
		VRInputBindingsComponent->InitInputForLocalVRPlayer(PlayerInputComponent, PC->GetLocalPlayer(), this);
}

void AVRPawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled() && !MainCamera->GetLastHMDLocation().IsZero())
	{
		UpdateMotionControllersLocations();
		if (bReplicates && GetNetMode() > ENetMode::NM_DedicatedServer)
			ReplicateVRDataToServer(DeltaTime);
	}
}

void AVRPawnBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		if (RightHand) RightHand->Destroy(true);
		if (LeftHand) LeftHand->Destroy(true);
	}

	Super::EndPlay(EndPlayReason);
}

void AVRPawnBase::UpdateMotionControllersLocations()
{
	TArray<IMotionController*> MotionControllers = IModularFeatures::Get().GetModularFeatureImplementations<IMotionController>(IMotionController::GetModularFeatureName());
	for (auto MotionController : MotionControllers) // TODO MotionControllers.Num() is always 1, should something be changed?
	{
		if (MotionController == nullptr) continue;

		FVector OutLocation;
		FRotator OutRotator;

		if (bUpdateGripLocations)
		{
			if (MotionController->GetControllerOrientationAndPosition(0, Grip_Right, OutRotator, OutLocation, 100.f))
				UpdateControllerLocation(GripLocation_RightHand, OutLocation, OutRotator);
			if (MotionController->GetControllerOrientationAndPosition(0, Grip_Left, OutRotator, OutLocation, 100.f))
				UpdateControllerLocation(GripLocation_LeftHand, OutLocation, OutRotator);
		}
		if (bUpdateAimLocations)
		{
			if (MotionController->GetControllerOrientationAndPosition(0, Aim_Right, OutRotator, OutLocation, 100.f))
				UpdateControllerLocation(AimLocation_RightHand, OutLocation, OutRotator);
			if (MotionController->GetControllerOrientationAndPosition(0, Aim_Left, OutRotator, OutLocation, 100.f))
				UpdateControllerLocation(AimLocation_LeftHand, OutLocation, OutRotator);
		}
	}
}

void AVRPawnBase::SwitchVRControllers(const TSubclassOf<AVRControllerBase>& LeftHandClass, const TSubclassOf<AVRControllerBase>& RightHandClass)
{
	if(LeftHandClass.Get()) SwitchVRController(LeftHandClass, false);
	if(RightHandClass.Get()) SwitchVRController(RightHandClass, true);
}

UVRBaseMovementComponent* AVRPawnBase::GetVRMovementComponent()
{ 
	if (IsValid(VRMovementComponent)) return VRMovementComponent;
	else VRMovementComponent = Cast<UVRBaseMovementComponent>(GetMovementComponent());
	return VRMovementComponent;
};

#pragma region Protected

void AVRPawnBase::UpdateControllerLocation(USceneComponent* UpdatingComponent, const FVector& NewLocation, const FRotator& NewRotation)
{
	FVector RelativeToHMDLocation = NewLocation - MainCamera->GetLastHMDLocation();
	UpdatingComponent->SetRelativeLocationAndRotation(MainCamera->GetRelativeLocation() + RelativeToHMDLocation, NewRotation);
}

void AVRPawnBase::SwitchVRController(const TSubclassOf<AVRControllerBase>& HandClass, bool bIsRightHand)
{
	auto ChosenHand = bIsRightHand ? RightHand : LeftHand;
	if (ChosenHand) ChosenHand->Destroy(true);

	USceneComponent* SpawnPoint = bIsRightHand ? GripLocation_RightHand : GripLocation_LeftHand;
	FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	SpawnParams.Owner = this;

	ChosenHand = GetWorld()->SpawnActor<AVRControllerBase>(HandClass, SpawnPoint->GetComponentLocation(), SpawnPoint->GetComponentRotation(), SpawnParams);
	if (ChosenHand) // ChosenHand will be nullptr if HandClass was null
	{
		ChosenHand->Setup(this, bIsRightHand);
		if (bIsRightHand) RightHand = ChosenHand; else LeftHand = ChosenHand;
	}
}

#pragma endregion

#pragma region IPlayerInputReceiver_interface

TScriptInterface<IPlayerInputReceiver> AVRPawnBase::GetInputReceiver_Implementation(bool IsRightHand) const
{
	if (IsRightHand && RightHand) return RightHand->GetControllerState();
	else if (LeftHand) return LeftHand->GetControllerState();
	return nullptr;
}

#pragma endregion

#pragma region Multiplayer_related_overrides

void AVRPawnBase::PawnClientRestart()
{
	UE_LOG(VRBaseLog, Log, TEXT("%s"), *FString(__FUNCTION__));
	Super::PawnClientRestart(); // ! Gets called on the server for locally controlled pawn ! See APlayerController::OnPossess
	if (IsLocallyControlled()) OnPawnReadyToPlay_Internal(true);
}

void AVRPawnBase::OnRep_PlayerState()
{
	UE_LOG(VRBaseLog, Log, TEXT("%s"), *FString(__FUNCTION__));
	Super::OnRep_PlayerState();
	if (!IsLocallyControlled()) OnPawnReadyToPlay_Internal(false);
}

void AVRPawnBase::PossessedBy(AController* NewController)
{
	UE_LOG(VRBaseLog, Log, TEXT("%s"), *FString(__FUNCTION__));
	Super::PossessedBy(NewController);

	if(bSpawnControllersOnPossess) SwitchVRControllers(LeftHandControllerClass, RightHandControllerClass);

	// Local controller will call OnPawnReadyToPlay_Internal from PawnClientRestart (even NM_Standalone). Weird
	if(!NewController->IsLocalPlayerController()) OnPawnReadyToPlay_Internal(false);
}

void AVRPawnBase::OnPawnReadyToPlay_Internal(bool bLocalPlayer)
{
	UE_LOG(VRBaseLog, Log, TEXT("%s - IsLocalPlayer: %d, Left/Right hands: %d/%d"), *FString(__FUNCTION__), bLocalPlayer ? 1 : 0, LeftHand ? 1 : 0, RightHand ? 1 : 0);
	if (bLocalPlayer)
	{
		// Can be somewhere else, only needeed to be called once in XRSystem`s lifetime
		if (GEngine && GEngine->XRSystem.IsValid()) GEngine->XRSystem->SetTrackingOrigin(EHMDTrackingOrigin::Floor);

		if (auto PlayerController = Cast<APlayerController>(GetController())) // Multiplayer will utilize that call
			if (PlayerController->Implements<UNotifiable>()) INotifiable::Execute_ReceiveNotification_NoParams(PlayerController); 
	}
	else
	{
		VRBaseDataReplicationComponent->SetComponentTickEnabled(true); // Only non Local player needs to update rep data
	}

	OnPawnReadyToPlay(bLocalPlayer); // BP event call
}

void AVRPawnBase::ReplicateVRDataToServer(float DeltaTime)
{
	auto DataToReplicate = FVRBasePlayerData();
	if (VRBaseDataReplicationComponent->GetLocalDataToReplicate(DeltaTime, DataToReplicate))
		Server_UpdateVRBaseData(DataToReplicate);
}

#pragma endregion

#pragma region Network_support

void AVRPawnBase::OnRep_Hand_Left()
{
	UE_LOG(VRBaseLog, Log, TEXT("%s - Valid Left hand data received: %d"), *FString(__FUNCTION__), LeftHand ? 1 : 0);
	// Pawn and its hands get spawned in the same frame. There is no guarantee which actor will be received by the client first, so hands do setup from two places
	if (LeftHand) LeftHand->Setup(this, false);
}

void AVRPawnBase::OnRep_Hand_Right()
{
	UE_LOG(VRBaseLog, Log, TEXT("%s - Valid Right hand data received: %d"), *FString(__FUNCTION__), RightHand ? 1 : 0);
	if (RightHand) RightHand->Setup(this, true);
}

void AVRPawnBase::Server_UpdateVRBaseData_Implementation(const FVRBasePlayerData& NewReplicatedBaseData)
{
	ReplicatedBaseData = MoveTemp(const_cast<FVRBasePlayerData&>(NewReplicatedBaseData)); // TODO is that the correct way to reduce allocations?
	OnRep_BaseVRData();
}

void AVRPawnBase::OnRep_BaseVRData()
{
	VRBaseDataReplicationComponent->SetNewVRData(ReplicatedBaseData);
}

void AVRPawnBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AVRPawnBase, ReplicatedBaseData, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AVRPawnBase, LeftHand, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AVRPawnBase, RightHand, COND_OwnerOnly);
}

#pragma endregion