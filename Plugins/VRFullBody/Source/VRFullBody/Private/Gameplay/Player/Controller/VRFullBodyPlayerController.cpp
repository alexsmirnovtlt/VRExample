// 2021-2022 Alexander Smirnov


#include "Gameplay/Player/Controller/VRFullBodyPlayerController.h"

#include "Gameplay/Player/Pawns/VRFullBodyPawn.h"
#include "Gameplay/Actors/FullBodyAvatar.h"

AVRFullBodyPlayerController::AVRFullBodyPlayerController()
{
	AvatarData = FVRFullBodyAvatarData();
}

void AVRFullBodyPlayerController::OnPossess(APawn* aPawn)
{
	if(IsFullBodyAvatar) SpawnAvatar(aPawn);
	Super::OnPossess(aPawn);
}

void AVRFullBodyPlayerController::Destroyed()
{
	if (AuthAvatarActor) AuthAvatarActor->Destroy();
	Super::Destroyed();
}

void AVRFullBodyPlayerController::SpawnAvatar(APawn* aPawn)
{
	auto FullBodyPawn = Cast<AVRFullBodyPawn>(aPawn);
	ensure(FullBodyPawn && FullBodyPawn->FullBodyAvatarClass);
	if (FullBodyPawn && FullBodyPawn->FullBodyAvatarClass)
	{
		FullBodyPawn->SetSpawnControllersOnPossess(false);

		// Spawn an Avatar representation and init it so its ready to play immediately for the server and when either pawn`s or Avatar`s OnRep happens for clients
		FActorSpawnParameters SpawnParams = FActorSpawnParameters();
		SpawnParams.Owner = FullBodyPawn;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AuthAvatarActor = GetWorld()->SpawnActor<AFullBodyAvatar>(
			FullBodyPawn->FullBodyAvatarClass, FullBodyPawn->GetFloorLevelComponent()->GetComponentLocation(),
			FullBodyPawn->AvatarRootGimbal->GetComponentRotation(), SpawnParams);
		AuthAvatarActor->AvatarData = this->AvatarData; // should be copied to pass that data from auth controllers to clients
		AuthAvatarActor->FullbodyPawn = FullBodyPawn;

		FullBodyPawn->Avatar = AuthAvatarActor;
		AuthAvatarActor->OnRep_ReceiveAvatarData(); // Setup avatar for player in authority, all needed data is present
	}
}

void AVRFullBodyPlayerController::HotSwapAvatarData(FVRFullBodyAvatarData NewAvatarData)
{
	ensure(IsFullBodyAvatar && GetNetMode() == ENetMode::NM_Standalone);
	AvatarData = NewAvatarData;
	if (AuthAvatarActor) AuthAvatarActor->Destroy();
	if (IsFullBodyAvatar) SpawnAvatar(GetPawn());
}