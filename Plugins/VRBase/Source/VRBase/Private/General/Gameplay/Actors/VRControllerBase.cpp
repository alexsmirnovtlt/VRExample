// 2021-2022 Alexander Smirnov


#include "General/Gameplay/Actors/VRControllerBase.h"

#include "Components/WidgetInteractionComponent.h"
#include "Components/ShapeComponent.h"
#include "Net/UnrealNetwork.h"

#include "General/Gameplay/Interaction/Player/DistantInteractionComponent.h"
#include "General/Gameplay/Interaction/Player/GrabComponent.h"
#include "General/Gameplay/Objects/VRControllerState.h"
#include "General/Gameplay/Pawns/VRPawnBase.h"
#include "VRBase.h"

AVRControllerBase::AVRControllerBase(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	bReplicates = true;
	SetReplicatingMovement(false); // Ignore attachment (important!) and movement replication
	bNetUseOwnerRelevancy = true;
	NetUpdateFrequency = 0.f; // Will be managed by owning VR pawn

	NiagaraDefaultParameterName = FName("User.PointArray");
	bAttachToGripAsProxy = true;

	OwningVRPawn = nullptr;
	ControllerState = nullptr;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// TODO make this component a custom non scene component, since its interaction mode is always custom
	WidgetInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteractionComponent"));
	WidgetInteractionComponent->SetupAttachment(RootComponent);
	WidgetInteractionComponent->InteractionSource = EWidgetInteractionSource::Custom;
#if WITH_EDITORONLY_DATA
	WidgetInteractionComponent->SetRelativeScale3D(FVector(0.05f)); // To hide that HUGE arrow in the editor
#endif

	DistantInteractionComponent = CreateDefaultSubobject<UDistantInteractionComponent>(TEXT("DistantInteractionLogic"));
	GrabComponent = CreateDefaultSubobject<UGrabComponent>(TEXT("HandGrabLogic"));
}

void AVRControllerBase::BeginPlay()
{
	Super::BeginPlay();

	// Important note for clients: replicated data with COND_InitialOnly may or may not be available when BeginPlay() executes.
	// So 'actual' BeginPlay() for them is OnInitialBunchDataReceived(), which executes when data gets received (OnRep), so all initial bunch data should be in a single struct to guarantee correct work
	// Make sure to fill that struct with data on a server
	
	// btw the order in which client receives pawns and hands is also undefined.  

	if (OwningVRPawn)
	{
		if (bIsRightHand) { UE_LOG(VRBaseLog, Log, TEXT("%s - with valid data for Right Hand"), *FString(__FUNCTION__)); }
		else { UE_LOG(VRBaseLog, Log, TEXT("%s - with valid data for Left Hand"), *FString(__FUNCTION__)); }
	}
	else { UE_LOG(VRBaseLog, Log, TEXT("%s - with invalid data"), *FString(__FUNCTION__)); }
}

void AVRControllerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ControllerState) ControllerState->OnStateExit();
	Super::EndPlay(EndPlayReason);
}

void AVRControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Hand movement, just teleports to grip location. Can be overriden to use with PhysConstraint or something else, hand overlaps stay the same
	if (auto GripLocation = OwningVRPawn->GetGripComponent(bIsRightHand)) SetActorLocationAndRotation(GripLocation->GetComponentLocation(), GripLocation->GetComponentRotation(), true);

	if (ControllerState)
	{
		GrabComponent->UpdateInteraction(DeltaTime, !ControllerState->IsGrabAllowed()); // ControllerState may become nullptr after that!
		if(ControllerState) DistantInteractionComponent->UpdateInteraction(DeltaTime, !ControllerState->IsDistantInteractionAllowed());
		if(ControllerState) ControllerState->Tick(DeltaTime);
	}
}

void AVRControllerBase::Setup_Implementation(AVRPawnBase* OwningPawn, bool bIsRight)
{
	UE_LOG(VRBaseLog, Log, TEXT("%s - Setup Start"), *FString(__FUNCTION__));

	if (!ensure(OwningPawn)) return;
	if (OwningVRPawn)
	{ // Setup was already performed
		if (bIsRightHand) { UE_LOG(VRBaseLog, Log, TEXT("%s - Right Hand Setup skipped, was already set up"), *FString(__FUNCTION__)); }
		else { UE_LOG(VRBaseLog, Log, TEXT("%s - Left Hand Setup skipped, was already set up"), *FString(__FUNCTION__)); }
		return; 
	}

	OwningVRPawn = OwningPawn;
	bIsRightHand = bIsRight;

	if (bIsRightHand) { UE_LOG(VRBaseLog, Log, TEXT("%s - Seting up Right Hand"), *FString(__FUNCTION__)); }
	else { UE_LOG(VRBaseLog, Log, TEXT("%s - Seting up Left Hand"), *FString(__FUNCTION__)); }

	if (GetNetMode() == ENetMode::NM_DedicatedServer || GetNetMode() == ENetMode::NM_ListenServer)
	{ // Copy this data to the struct for initial bunch replication
		InitialBunchData_HandController = FInitialBunchData_HandController(OwningVRPawn, bIsRightHand);
	}
	
	SetupForLocalPlayerOrSimulatedProxy();
}

void AVRControllerBase::SetupForLocalPlayer_Implementation()
{
	UE_LOG(VRBaseLog, Log, TEXT("%s"), *FString(__FUNCTION__));
	AddTickPrerequisiteActor(OwningVRPawn);
	SetActorTickEnabled(true);

	// Init interaction and grab components
	DistantInteractionComponent->SetupComponent(this, WidgetInteractionComponent);
	if (!bIsRightHand) WidgetInteractionComponent->PointerIndex = 1;

	if (auto GrabCollider = GetGrabCollisionPrimitive())
		GrabComponent->SetupComponent(this, GrabCollider);
	else GrabComponent->SetKeepUpdatingInteraction(false);
	//
	ChangeState(StartingState, false);
}

void AVRControllerBase::ChangeState(const TSubclassOf<UVRControllerState>& NewStateClass, bool NotifyPairedControllerIfAble)
{
	UClass* PreviousState = nullptr;
	if (ControllerState)
	{
		PreviousState = ControllerState ? ControllerState->GetClass() : nullptr;
		ControllerState->OnStateExit();
		ControllerState = nullptr; // if new class is not valid, gc current anyway
	}

	if (!ensure(NewStateClass)) return;
	ControllerState = NewObject<UVRControllerState>(this, NewStateClass);
	if (ControllerState)
	{
		ControllerState->Setup(this, PreviousState);
		ControllerState->OnStateEnter();

		if (NotifyPairedControllerIfAble)
		{
			if (auto AnotherHand = GetAnotherHandController())
			{
				if(auto AnotherState = AnotherHand->GetControllerState())
					AnotherState->OnOtherControllerStateChanged(ControllerState);
			}
		}
	}
}

void AVRControllerBase::ChangeToDefaultState(bool NotifyPairedControllerIfAble)
{
	if (!ensure(StartingState)) return;
	ChangeState(StartingState, NotifyPairedControllerIfAble);
}

void AVRControllerBase::DoAimLineTrace(ECollisionChannel ObjectQueryParams, float TraceDistance, FHitResult& HitInfo)
{
	// TODO Maybe should be cached, because DistantInteractionComponent calls it every frame

	auto AimComponent = GetAimComponent();
	if (!AimComponent) return;

	FVector TraceStart = AimComponent->GetComponentLocation();
	FVector TraceEnd = TraceStart + AimComponent->GetForwardVector() * TraceDistance;
	
	FCollisionQueryParams QueryParams = FCollisionQueryParams();
	QueryParams.bTraceComplex = false;
	// Adding both hands and a pawn to ignore list
	QueryParams.AddIgnoredActor(this);
	if (OwningVRPawn)
	{
		QueryParams.AddIgnoredActor(OwningVRPawn);
		if (auto AnotherHand = GetAnotherHandController()) QueryParams.AddIgnoredActor(AnotherHand);
	}

	FCollisionObjectQueryParams ObjectParams = FCollisionObjectQueryParams(ObjectQueryParams);
	GetWorld()->LineTraceSingleByObjectType(HitInfo, TraceStart, TraceEnd, ObjectParams, QueryParams);
}

AVRControllerBase* AVRControllerBase::GetAnotherHandController() const
{
	return OwningVRPawn ? OwningVRPawn->GetHandController(!bIsRightHand) : nullptr;
}

USceneComponent* AVRControllerBase::GetAimComponent_Implementation() const
{
	return OwningVRPawn ? OwningVRPawn->GetAimComponent(bIsRightHand) : nullptr;
}

void AVRControllerBase::SetupForLocalPlayerOrSimulatedProxy()
{
	ensure(OwningVRPawn);
	// Either setup actor for client or skip setup and attach this actor to grip location
	if (OwningVRPawn->IsLocallyControlled()) SetupForLocalPlayer();
	// For non local player - attach to pawn's grip, do not do any setup or tick
	else if (bAttachToGripAsProxy) AttachToOwnerGrip();
}

void AVRControllerBase::AttachToOwnerGrip()
{
	UE_LOG(VRBaseLog, Log, TEXT("%s"), *FString(__FUNCTION__));
	AttachToComponent(
		OwningVRPawn->GetGripComponent(bIsRightHand),
		FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false)
	);
}

// Networking support

void AVRControllerBase::OnRep_InitialBunchDataReceived()
{
	if (!IsValid(InitialBunchData_HandController.OwningVRPawn))
	{
		if (InitialBunchData_HandController.bIsRightHand)
		{ UE_LOG(VRBaseLog, Log, TEXT("%s - Invalid pawn data received for Right Hand"), *FString(__FUNCTION__));}
		else { UE_LOG(VRBaseLog, Log, TEXT("%s - Invalid pawn data received for Left Hand"), *FString(__FUNCTION__));}
		return; // Pawn itself was not replicated yet, that pawn will init this controller by itself after by calling Setup from OnRep_Hand_Right(or Left)
	}
	OwningVRPawn = InitialBunchData_HandController.OwningVRPawn;
	bIsRightHand = InitialBunchData_HandController.bIsRightHand;

	if (InitialBunchData_HandController.bIsRightHand)
	{ UE_LOG(VRBaseLog, Log, TEXT("%s - Valid pawn data received for Right Hand"), *FString(__FUNCTION__));}
	else { UE_LOG(VRBaseLog, Log, TEXT("%s - Valid pawn data received for Left Hand"), *FString(__FUNCTION__)); }

	SetupForLocalPlayerOrSimulatedProxy();
}

void AVRControllerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AVRControllerBase, InitialBunchData_HandController, COND_InitialOnly);
}