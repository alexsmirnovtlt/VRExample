// 2021-2022 Alexander Smirnov


#include "Gameplay/Player/ActorComponents/MultiplayerVOIPTalker.h"

#include "Gameplay/Interfaces/VoipTalkerStatusReceiver.h"

void UMultiplayerVOIPTalker::BeginPlay()
{
	Super::BeginPlay();

	if (auto OwningActor = GetOwner())
	{
		if (OwningActor->Implements<UVoipTalkerStatusReceiver>())
			ValidOwner = OwningActor;
	}

	bIsNowTalking = false;
}

void UMultiplayerVOIPTalker::OnTalkingBegin(UAudioComponent* AudioComponent)
{
	if (ValidOwner) IVoipTalkerStatusReceiver::Execute_OnTalkingStateChanged(ValidOwner, true);
	BPOnTalkingBegin(AudioComponent);
	bIsNowTalking = true;
}

void UMultiplayerVOIPTalker::OnTalkingEnd()
{
	if (ValidOwner) IVoipTalkerStatusReceiver::Execute_OnTalkingStateChanged(ValidOwner, false);
	BPOnTalkingEnd();
	bIsNowTalking = false;
}