// 2021-2022 Alexander Smirnov


#include "Gameplay/Player/Pawns/VRFullBodyPawn.h"

#include "Net/UnrealNetwork.h"

#include "General/Gameplay/Movement/ActorComponents/VRCameraComponent.h"
#include "Gameplay/ActorComponents/VRFullBodyReplicationComponent.h"
#include "Gameplay/Actors/FullBodyAvatar.h"

AVRFullBodyPawn::AVRFullBodyPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UVRFullBodyReplicationComponent>("VRBaseDataReplication"))
{
	FullBodyAvatarClass = AFullBodyAvatar::StaticClass();

	bSpawnControllersOnPossess = false;

	AvatarRootGimbal = CreateDefaultSubobject<USceneComponent>(TEXT("AvatarRootGimbal"));
	AvatarRootGimbal->SetupAttachment(FloorLevelComponent);

	AvatarRoot = CreateDefaultSubobject<USceneComponent>(TEXT("AvatarRoot"));
	AvatarRoot->SetupAttachment(AvatarRootGimbal);

	LocalPlayerHeadBone = CreateDefaultSubobject<USceneComponent>(TEXT("LocalPlayerHeadBone"));
	LocalPlayerHeadBone->SetupAttachment(MainCamera);

	LocalPlayerBone_Hand_Right = CreateDefaultSubobject<USceneComponent>(TEXT("LocalPlayerBone_Hand_Right"));
	LocalPlayerBone_Hand_Right->SetupAttachment(GripLocation_RightHand);
	LocalPlayerBone_Hand_Left = CreateDefaultSubobject<USceneComponent>(TEXT("LocalPlayerBone_Hand_Left"));
	LocalPlayerBone_Hand_Left->SetupAttachment(GripLocation_LeftHand);
}

void AVRFullBodyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Since pawn`s rotation logic is not straightforward and may differ between clients and even server, rotate Avatar Root manually so it always follows camera
	AvatarRootGimbal->SetRelativeRotation(FRotator(0.f, MainCamera->GetRelativeRotation().Yaw, 0.f));
}

void AVRFullBodyPawn::SetAvatarReferenceOnce(AFullBodyAvatar* NewPtr)
{
	if (!IsValid(Avatar)) Avatar = NewPtr;
}

FRotator AVRFullBodyPawn::GetPlayerForwardWorldRotation() const
{
	return FRotator(0, AvatarRootGimbal->GetComponentRotation().Yaw, 0);
}

void AVRFullBodyPawn::OnRep_AvatarActor()
{
	if (IsValid(Avatar)) Avatar->SetupAvatar(this);
}

void AVRFullBodyPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AVRFullBodyPawn, Avatar, COND_InitialOnly);
}