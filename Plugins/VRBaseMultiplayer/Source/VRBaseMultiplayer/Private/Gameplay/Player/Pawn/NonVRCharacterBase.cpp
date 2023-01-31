// 2021-2022 Alexander Smirnov


#include "Gameplay/Player/Pawn/NonVRCharacterBase.h"

#include "Components/WidgetInteractionComponent.h"
#include "Runtime/Launch/Resources/Version.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Online.h"

#include "Gameplay/Player/ActorComponents/NonVRCharacterMovementComponent.h"
#include "Gameplay/Player/Controllers/VRMultiplayerPlayerController.h"
#include "Gameplay/Player/ActorComponents/MultiplayerVOIPTalker.h"
#include "Subsystems/GameInstance/OnlineSessionSubsystem.h"
#include "General/Interfaces/Notifiable.h"

ANonVRCharacterBase::ANonVRCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UNonVRCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SimulatedProxyCameraLerpModifier = 1.f;
	MouseSensitivityScale = 0.7f;

	InteractionDistance = 1000.f;
	InteractionChannel = ECollisionChannel::ECC_WorldDynamic;
	bTraceComplex = false;

	// VR Camera
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	MainCamera->SetupAttachment(RootComponent);
	MainCamera->SetRelativeLocation(FVector(0.f, 0.f, 78.f));

	// Interaction with UIs
	WidgetInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteractionComponent"));
	WidgetInteraction->SetupAttachment(RootComponent);
	WidgetInteraction->TraceChannel = ECollisionChannel::ECC_WorldDynamic;
	WidgetInteraction->InteractionSource = EWidgetInteractionSource::Custom;

	bUseControllerRotationYaw = true;
	
	VOIPTalker = CreateDefaultSubobject<UMultiplayerVOIPTalker>(TEXT("VOIPTalker"));
}

void ANonVRCharacterBase::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);	
	EndInteraction();
}

void ANonVRCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update Camera Rotation for local player or simulated proxy
	if (Controller && Controller->IsLocalController())
	{
		if (auto PController = CastChecked<APlayerController>(Controller))
			MainCamera->SetRelativeRotation(FRotator(PController->GetControlRotation().Pitch, 0.f, 0.f));

		LineTraceForInteractableObjects(); // And fire interaction events on current obejct if able
	}
	else
	{
		MainCamera->SetRelativeRotation(
			GetSmoothedRemoteViewPitchRotation(MainCamera->GetRelativeRotation(), DeltaTime, SimulatedProxyCameraLerpModifier)
		);
	}
}

void ANonVRCharacterBase::PawnClientRestart()
{
	Super::PawnClientRestart();
	if (IsLocallyControlled()) OnPawnReadyToPlay_Internal(true);
}

void ANonVRCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (!IsLocallyControlled()) OnPawnReadyToPlay_Internal(false);
}

void ANonVRCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (!NewController->IsLocalPlayerController()) OnPawnReadyToPlay_Internal(false);
}

AVRMultiplayerPlayerController* ANonVRCharacterBase::GetMultiplayerPlayerController()
{
	if (auto PController = GetController())
	{
		return Cast<AVRMultiplayerPlayerController>(PController);
	}
	return nullptr;
}

void ANonVRCharacterBase::LineTraceForInteractableObjects()
{
	FHitResult HitResult;
	FVector TraceStart = MainCamera->GetComponentLocation();
	FVector TraceEnd = TraceStart + MainCamera->GetForwardVector() * InteractionDistance;

	FCollisionQueryParams QueryParams = FCollisionQueryParams();
	QueryParams.bTraceComplex = bTraceComplex;
	QueryParams.AddIgnoredActor(this);

	FCollisionObjectQueryParams ObjectParams = FCollisionObjectQueryParams(InteractionChannel);
	GetWorld()->LineTraceSingleByObjectType(HitResult, TraceStart, TraceEnd, ObjectParams, QueryParams);

	WidgetInteraction->SetCustomHitResult(HitResult);

	if (HitResult.IsValidBlockingHit())
	{
		if(!HitResult.GetActor() ||
			!HitResult.GetActor()->Implements<UNonVRInteractable>()) {
			EndInteraction(); return;
		}
		AActor* HitActor = HitResult.GetActor();

		float ObjectInteractionDistance = INonVRInteractable::Execute_NonVR_GetInteractionDistance(HitActor);
		if (ObjectInteractionDistance > 0.f && HitResult.Distance > InteractionDistance) { EndInteraction(); return; }

		if (IsValid(InteractableObject.GetObject()))
		{
			if (Cast<UObject>(HitActor) == InteractableObject.GetObject())
			{
				// Same object as before, just execute tick
				INonVRInteractable::Execute_NonVR_InteractionTick(InteractableObject.GetObject(), this, HitResult);
			}
			else
			{
				EndInteraction();
				InteractableObject.SetObject(HitActor);
				INonVRInteractable::Execute_NonVR_InteractionStarted(InteractableObject.GetObject(), this, HitResult);
			}
		}
		else
		{
			InteractableObject.SetObject(HitActor);
			INonVRInteractable::Execute_NonVR_InteractionStarted(InteractableObject.GetObject(), this, HitResult);
		}

	}
	else EndInteraction();
}

void ANonVRCharacterBase::EndInteraction()
{
	if (WidgetInteraction->GetHoveredWidgetComponent()) // means that CustomHitResult is a valid widget
	{
		FHitResult DefaultHitResult;
		WidgetInteraction->SetCustomHitResult(DefaultHitResult);
		//ClearSlateWidgetReference();
	}

	auto CurrentObject = InteractableObject.GetObject();
	if(!IsValid(CurrentObject)) return;

	INonVRInteractable::Execute_NonVR_InteractionEnded(CurrentObject, this);
	InteractableObject.SetObject(nullptr);
}

void ANonVRCharacterBase::ClearSlateWidgetReference()
{
	FWidgetPath EmptyWidgetPath;
	FPointerEvent PointerEvent(
		WidgetInteraction->VirtualUserIndex,
		WidgetInteraction->PointerIndex,
		FVector2D(),
		FVector2D(),
		TSet<FKey>(),
		FKey(),
		0.0f,
		FModifierKeysState());

	FSlateApplication::Get().RoutePointerMoveEvent(EmptyWidgetPath, PointerEvent, false);
}

FRotator ANonVRCharacterBase::GetSmoothedRemoteViewPitchRotation(FRotator CurrenRotation, float DeltaTime, float LerpModifier)
{
	FRotator NewRotation = FRotator(RemoteViewPitch * 360.0f / 255.0f, 0.f, 0.f);
	return UKismetMathLibrary::RLerp(CurrenRotation, NewRotation, DeltaTime * LerpModifier, true);
}

void ANonVRCharacterBase::OnPawnReadyToPlay_Internal(bool bLocalPlayer)
{
	SetActorTickEnabled(true);

	if (bLocalPlayer)
	{
		if (auto PlayerController = Cast<APlayerController>(GetController())) // Multiplayer will utilize that call
			if (PlayerController->Implements<UNotifiable>()) INotifiable::Execute_ReceiveNotification_NoParams(PlayerController);
	}
	else
	{
		if (GetNetMode() != ENetMode::NM_Standalone)
		{
			VOIPTalker->Settings.ComponentToAttachTo = MainCamera;
			VOIPTalker->Settings.AttenuationSettings = VOIPAttenuation;

			VOIPTalker->RegisterWithPlayerState(GetPlayerState());
		}
	}

	OnPawnReadyToPlay(bLocalPlayer); // Call to BP
}

void ANonVRCharacterBase::LeaveOnlineSession()
{
	if (GetNetMode() == ENetMode::NM_Standalone) return;

	if (auto OnlineSessionSubsys = GetGameInstance()->GetSubsystem<UOnlineSessionSubsystem>())
	{
		DestroySessionHandle = OnlineSessionSubsys->OnOnlineSessionDestroyed_Event.AddUObject(this, &ANonVRCharacterBase::OnOnlineSessionDestroyed);
		OnlineSessionSubsys->DestroySession();
	}	
}

void ANonVRCharacterBase::OnOnlineSessionDestroyed(bool bSuccess)
{
	if (auto OnlineSessionSubsys = GetGameInstance()->GetSubsystem<UOnlineSessionSubsystem>())
	{
		OnlineSessionSubsys->OnOnlineSessionDestroyed_Event.Remove(DestroySessionHandle);
		DestroySessionHandle.Reset();
		if (bSuccess) OnOnlineSessionClosed();
	}
}

void ANonVRCharacterBase::CleanupVoiceData()
{
	if (GetNetMode() == ENetMode::NM_Standalone) return;

	auto VoiceInterface = Online::GetVoiceInterface();
	if (!VoiceInterface) return;

	auto PC = GetMultiplayerPlayerController();
	bool IsLocalPlayer = PC ? PC->IsLocalPlayerController() : false;
	if (IsLocalPlayer)
	{
		UVOIPStatics::ClearAllSettings();
		VoiceInterface->StopNetworkedVoice(0);
		VoiceInterface->UnregisterLocalTalkers();
		VoiceInterface->RemoveAllRemoteTalkers();
	}
	else if (auto PState = GetPlayerState())
	{
		// TODO Apparently on a server there is no PlayerState for the client at this point but UnregisterRemoteTalker() triggers for them somewhere anyway, so that fine?
		const FUniqueNetIdRepl& UniqueNetIdRepl = PState->GetUniqueId();
		if (UniqueNetIdRepl.IsValid())
		{
			UVOIPStatics::ResetPlayerVoiceTalker(UniqueNetIdRepl);
			VoiceInterface->UnregisterRemoteTalker(*UniqueNetIdRepl.GetUniqueNetId().Get());
		}
	}
}