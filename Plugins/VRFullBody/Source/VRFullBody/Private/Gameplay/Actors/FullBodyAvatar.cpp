// 2021-2022 Alexander Smirnov


#include "Gameplay/Actors/FullBodyAvatar.h"

#include "Net/UnrealNetwork.h"
#include "Engine/AssetManager.h"
#include "Engine/SkeletalMesh.h"
#include "Components/SkeletalMeshComponent.h"

#include "Gameplay/CoreManagers/Subsystems/GameInstance/PersistentVRDataSubsystem.h"
#include "General/Gameplay/Movement/ActorComponents/VRCameraComponent.h"
#include "Gameplay/Player/Controller/VRFullBodyPlayerController.h"
#include "Gameplay/Player/Pawns/VRFullBodyPawn.h"
#include "Settings/VRFullBodyDataSaveGame.h"
#include "Settings/AvatarDataAsset.h"
#include "VRFullBody.h"

// TODO This actor updates its location locally. Because of that relative to the pawn location may differ (need testing) between server and clients so it may be a good idea to send some adjustments in long intervals (1-3 seconds)

AFullBodyAvatar::AFullBodyAvatar()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	SetReplicateMovement(false);
	SetActorTickEnabled(false);

	DistanceToMoveThreshold = 8.f;
	RotationThreshold = 45.f;
	TeleportThreshold = 50.f;
	MatchPawnLocationSpeed = 80.f;
	MatchPawnLocationStopTolerance = 0.05f;
	MatchPawnRotationSpeed = 25.f;
	MatchPawnRotationStopTolerance = 0.05f;

	bWasSetup = false;
	bHasValidAvatarData = false;
	bIsMovingTowardsPawn = false;
	bIsInCalibrationState = false;
	bIsRotatingTowardsCamera = false;
	bAlignAvatarRotationWithPawnThisFrame = true;

	AvatarRoot = CreateDefaultSubobject<USceneComponent>(TEXT("AvatarRoot"));
	RootComponent = AvatarRoot;

	AvatarMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("AvatarMesh"));
	AvatarMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	AvatarMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AvatarMesh->SetupAttachment(AvatarRoot);
}

void AFullBodyAvatar::EndPlay(EEndPlayReason::Type Reason)
{
	if (AvatarAssetStreamableHandle.IsValid())
	{
		AvatarAssetStreamableHandle.Get()->CancelHandle();
		AvatarAssetStreamableHandle.Reset();
	}
	Super::EndPlay(Reason);	
}

void AFullBodyAvatar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bHasValidAvatarData && bWasSetup && FullbodyPawn && !FullbodyPawn->IsCameraDetached())
	{
		UpdateActorLocation(DeltaTime);
		UpdateActorRotation(DeltaTime);
	}
}

# pragma region Avatar_Setup

void AFullBodyAvatar::SetupAvatar(AVRFullBodyPawn* Pawn)
{
	if (Pawn) FullbodyPawn = Pawn; // Just set a reference to a pawn if a pawn got its pointer first, then wait for this class`s OnRep for setup
	if (!bHasValidAvatarData || bWasSetup) return;
	if (!Pawn && FullbodyPawn) FullbodyPawn->SetAvatarReferenceOnce(this);

	if (!AvatarData.AvatarAssetPrimaryID.IsEmpty())
		HotSwapAvatar(FPrimaryAssetId(AvatarData.AvatarAssetPrimaryID));

	bWasSetup = true;
}

void AFullBodyAvatar::HotSwapAvatar(UPARAM(Ref) const FPrimaryAssetId& Asset)
{
	LastPrimaryAssetID = Asset;
	if (auto AssetManager = UAssetManager::GetIfValid())
	{
		AvatarAssetStreamableHandle = AssetManager->LoadPrimaryAsset(
			Asset,
			TArray<FName>(),
			FStreamableDelegate::CreateUObject(this, &AFullBodyAvatar::OnAvatarAssetLoaded)
		);
	}
}

void AFullBodyAvatar::OnAvatarAssetLoaded()
{
	if (auto AssetManager = UAssetManager::GetIfValid())
	{
		AvatarAssetStreamableHandle.Reset();
		if (const auto Asset = AssetManager->GetPrimaryAssetObject<UAvatarDataAsset>(LastPrimaryAssetID))
			ApplyDataFromAvatarAsset(Asset);
		AssetManager->UnloadPrimaryAsset(LastPrimaryAssetID);
	}
}

void AFullBodyAvatar::ApplyDataFromAvatarAsset(UAvatarDataAsset* AssetData)
{
	if (!AssetData->Mesh) { ensure(false); return; }
	AvatarAssetData = AssetData;

	float AvatarScale = AvatarData.AvatarHeight / AssetData->MeshHeight;
	FullbodyPawn->AvatarRoot->SetRelativeLocation(FVector(AssetData->MeshOffset * AvatarScale, 0));
	SetActorLocation(FullbodyPawn->AvatarRoot->GetComponentLocation()); // Locally update avatar spawn location and Pawn`s Avatar Root location to have the correct offset between camera and Avatar Root (they are not the same XY location!)	
	// Reposition component on a pawn that will be an affector for the head bone
	FVector TPoseHeadBoneLocation = AssetData->HeadBoneTPoseTransform.GetLocation() * AvatarScale;
	FVector TPoseCameraLocation = FVector(0, 0, AvatarData.AvatarHeight - AvatarData.AvatarForeheadCameraOffset);
	UE_LOG(VRFullbodyLog, Warning, TEXT("%s: Mesh Scale: %f (%f/%f), Eye Height: %f"), *FString(__FUNCTION__), AvatarScale, AvatarData.AvatarHeight, AssetData->MeshHeight, AvatarData.AvatarHeight - AvatarData.AvatarForeheadCameraOffset);
	FVector CameraToBoneOffset = TPoseHeadBoneLocation - TPoseCameraLocation;
	FullbodyPawn->LocalPlayerHeadBone->SetRelativeLocationAndRotation(CameraToBoneOffset, AssetData->HeadBoneTPoseTransform.GetRotation());
	//
	bAlignAvatarRotationWithPawnThisFrame = true;
	LastPawnLocation = GetDesiredWorldLocation();
	SetActorTickEnabled(true);
	// Mesh setup
	AvatarMesh->SetRelativeTransform(AssetData->AvatarRootOffset); // Scale doesnt matter, see SetRelativeScale3D below
	AvatarMesh->SetSkeletalMesh(AssetData->Mesh);
	AvatarMesh->SetRelativeScale3D(FVector(AvatarScale));
	// Pawn setup
	FullbodyPawn->AdditionalYawAmountPerTick = AssetData->AvatarGimbalAdditionalYawOffset;
	FullbodyPawn->WaistForwardAxis = AssetData->SkeletonData.GetDefaultObject()->WaistForwardAxis;

	SetupAffectorsForLocalPlayer(AvatarScale); // Skip on simulated proxies, clients will receive locations as CameraComponent for the head bone and AimComponents for the hand bones

	if (AssetData->AnimInstance)
		AvatarMesh->SetAnimInstanceClass(AssetData->AnimInstance);

	OnAvatarChanged(); // BP call
}

void AFullBodyAvatar::SetupAffectorsForLocalPlayer(float AvatarScale)
{
	if (!FullbodyPawn->IsLocallyControlled()) return;
	if (auto PC = Cast<AVRFullBodyPlayerController>(FullbodyPawn->Controller))
		SetupAffectorsForLocalPlayer_Internal();
}

void AFullBodyAvatar::SetupAffectorsForLocalPlayer_Internal()
{
	auto SaveGameSubsystem = GetGameInstance()->GetSubsystem<UPersistentVRDataSubsystem>();
	auto SaveGameData = SaveGameSubsystem->GetSaveGameData();

	FTransform RelativeHandTransform_Right = SaveGameData->RightHandOffset.Equals(FTransform::Identity) ?
		AvatarAssetData->RightHandToBoneOffset : SaveGameData->RightHandOffset;
	FTransform RelativeHandTransform_Left = SaveGameData->LeftHandOffset.Equals(FTransform::Identity) ?
		AvatarAssetData->LeftHandToBoneOffset : SaveGameData->LeftHandOffset;

	FullbodyPawn->LocalPlayerBone_Hand_Right->SetRelativeTransform(RelativeHandTransform_Right);
	FullbodyPawn->LocalPlayerBone_Hand_Left->SetRelativeTransform(RelativeHandTransform_Left);
}

# pragma endregion

void AFullBodyAvatar::UpdateActorLocation(float DeltaTime)
{
	FVector NewLocation = GetActorLocation();
	FVector PawnLocation = GetDesiredWorldLocation();
	float Distance2D = FVector::Dist2D(NewLocation, PawnLocation);
	bool bIsPawnMovingWithInputOfFalling = !FullbodyPawn->GetVelocity().IsNearlyZero();

	if (Distance2D >= TeleportThreshold || bIsInCalibrationState)
	{
		bIsMovingTowardsPawn = false;
		NewLocation = GetDesiredWorldLocation();
		bAlignAvatarRotationWithPawnThisFrame = true;
	}
	else if (bIsMovingTowardsPawn)
	{
		// Pawn may be moving right now or stopped but we still need to match its location
		FVector AvatarMaxMovement = PawnLocation - NewLocation/*current location*/;
		AvatarMaxMovement.Z = 0;

		if (bIsPawnMovingWithInputOfFalling) // Compensate distance to the pawn so it wont fall behind with every frame (in other words ensure that distance between pawn and avatar is not encreasing with every frame)
			NewLocation += PawnLocation - LastPawnLocation;

		FVector ActualMovementForThisFrame = AvatarMaxMovement.GetSafeNormal() * MatchPawnLocationSpeed * DeltaTime;
		NewLocation += ActualMovementForThisFrame.GetClampedToMaxSize2D(AvatarMaxMovement.Size2D());

		if (FMath::Abs(NewLocation.SizeSquared2D() - PawnLocation.SizeSquared2D()) < MatchPawnLocationStopTolerance)
			bIsMovingTowardsPawn = false;
	}
	// Was standing still previous frame
	else if (Distance2D >= DistanceToMoveThreshold)
		bIsMovingTowardsPawn = true;

	NewLocation.Z = PawnLocation.Z; // Always match pawn`s height
	SetActorLocation(NewLocation);
	LastPawnLocation = MoveTemp(PawnLocation);
}

void AFullBodyAvatar::UpdateActorRotation(float DeltaTime)
{
	FRotator PlayerRotation = FullbodyPawn->GetPlayerForwardWorldRotation();
	// Assuming all rotators have 0 pitch and roll
	if (bAlignAvatarRotationWithPawnThisFrame)
	{
		bAlignAvatarRotationWithPawnThisFrame = false;
		TargetPawnRotation = PlayerRotation;
		SetActorRotation(PlayerRotation);
		return;
	}

	if (bIsMovingTowardsPawn)
	{
		bIsRotatingTowardsCamera = true;
		TargetPawnRotation = PlayerRotation;
	}

	FRotator AvatarRotation = GetActorRotation();
	float MaxRotationPerFrame;
	if (bIsRotatingTowardsCamera)
		MaxRotationPerFrame = (TargetPawnRotation - AvatarRotation).GetNormalized().Yaw;
	else 
		MaxRotationPerFrame = (PlayerRotation - AvatarRotation).GetNormalized().Yaw;

	float AbsRotationDifference = FMath::Abs(MaxRotationPerFrame);

	if (AbsRotationDifference > RotationThreshold/* && !bIsRotatingTowardsCamera*/)
	{
		TargetPawnRotation = PlayerRotation;
		bIsRotatingTowardsCamera = true;
	}

	if (bIsRotatingTowardsCamera)
	{
		float YawToAdd = FMath::Clamp(MaxRotationPerFrame * DeltaTime * MatchPawnRotationSpeed, -AbsRotationDifference, AbsRotationDifference);
		if (FMath::Abs(YawToAdd) < MatchPawnRotationStopTolerance) bIsRotatingTowardsCamera = false;
		AddActorWorldRotation(FRotator(0, YawToAdd, 0));

		bRotatingTowardsCamera_Clockwise = YawToAdd > 0.f;
	}
}

FVector AFullBodyAvatar::GetDesiredWorldLocation()
{
	return FullbodyPawn->AvatarRoot->GetComponentLocation();
}

void AFullBodyAvatar::OnRep_ReceiveAvatarData()
{
	bHasValidAvatarData = true;
	if (IsValid(FullbodyPawn)) SetupAvatar();
}

void AFullBodyAvatar::OnRep_ReceiveFullBodyPawn()
{
	if(bHasValidAvatarData && IsValid(FullbodyPawn)) SetupAvatar();
}

void AFullBodyAvatar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AFullBodyAvatar, AvatarData, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(AFullBodyAvatar, FullbodyPawn, COND_InitialOnly);
}