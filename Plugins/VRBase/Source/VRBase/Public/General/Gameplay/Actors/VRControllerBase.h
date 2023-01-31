// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "General/Input/Interfaces/PlayerInputReceiver.h"

#include "VRControllerBase.generated.h"

class UDistantInteractionComponent;
class UWidgetInteractionComponent;
class UPrimitiveComponent;
class UVRControllerState;
class UShapeComponent;
class UGrabComponent;
class AVRPawnBase;

USTRUCT()
struct VRBASE_API FInitialBunchData_HandController
{
	GENERATED_BODY()

	FInitialBunchData_HandController()
		: OwningVRPawn(nullptr), bIsRightHand(false) {}
	FInitialBunchData_HandController(AVRPawnBase* NewOwningVRPawn, bool NewbIsRightHand)
		: OwningVRPawn(NewOwningVRPawn), bIsRightHand(NewbIsRightHand) {}

	UPROPERTY(Transient)
	AVRPawnBase* OwningVRPawn;
	UPROPERTY(Transient)
	bool bIsRightHand;
};

UENUM()
enum HandNiagaraParticleType
{
	Undefined = 0 UMETA(DisplayName = "Undefined"),
	UIInteraction = 1 UMETA(DisplayName = "UIInteraction"),
	TeleportTrace = 2 UMETA(DisplayName = "TeleportTrace")
};

UCLASS(Abstract, hidecategories = ("Actor Tick", "Replication", "Rendering", "Collision", "LOD", "Cooking"))
class VRBASE_API AVRControllerBase : public AActor, public IPlayerInputReceiver
{
	GENERATED_BODY()
	
public:	
	AVRControllerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintNativeEvent, Category = "VR Base | Controller")
	void Setup(AVRPawnBase* OwningPawn, bool bIsRight);
	virtual void Setup_Implementation(AVRPawnBase* OwningPawn, bool bIsRight);
	UFUNCTION(BlueprintNativeEvent, Category = "VR Base | Controller")
	void SetupForLocalPlayer();
	virtual void SetupForLocalPlayer_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VR Base | Controller")
	UVRControllerState* GetControllerState() const { return ControllerState; };
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VR Base | Controller")
	AVRPawnBase* GetVRPawn() const { return OwningVRPawn; };
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VR Base | Controller")
	bool IsRightHand() const { return bIsRightHand; };

	UFUNCTION(BlueprintCallable, Category = "VR Base | Controller")
	AVRControllerBase* GetAnotherHandController() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VR Base | Controller")
	UDistantInteractionComponent* GetDistantInteractionComponent() const { return DistantInteractionComponent; };
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VR Base | Controller")
	UGrabComponent* GetGrabComponent() const { return GrabComponent; };

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Base | Controller")
	void SetHandVisibility(bool bVisible) const;
	void SetHandVisibility_Implementation(bool bVisible) { SetActorHiddenInGame(!bVisible); };

	// Optional Niagra particle systems for any other classes to get (may be used for teleport trace or UI beam)
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintImplementableEvent, Category = "VR Base | Controller")
	UNiagaraComponent* GetNiagaraSystem(HandNiagaraParticleType ParticleType);
	virtual UNiagaraComponent* GetNiagaraSystem_Implementation(HandNiagaraParticleType ParticleType) { return nullptr; }
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintImplementableEvent, Category = "VR Base | Controller")
	FName GetNiagaraParameterName(HandNiagaraParticleType ParticleType) const; //  TODO Doesnts work when not overridden!!
	virtual FName GetNiagaraParameterName_Implementation(HandNiagaraParticleType ParticleType) const { return NiagaraDefaultParameterName; }
	//

	// Do a trace from hand`s Aim location
	UFUNCTION(BlueprintCallable, Category = "VR Base | Controller")
	void DoAimLineTrace(ECollisionChannel ObjectQueryParams, float TraceDistance, FHitResult& HitInfo);

	// Primitive that overlaps other actors for GrabComponent to handle
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "VR Base | Controller")
	UShapeComponent* GetGrabCollisionPrimitive() const;
	virtual UShapeComponent* GetGrabCollisionPrimitive_Implementation() const { return nullptr; };

	UFUNCTION(BlueprintCallable, Category = "VR Base | Controller")
	void ChangeState(const TSubclassOf<UVRControllerState>& NewStateClass, bool NotifyPairedControllerIfAble = true);
	UFUNCTION(BlueprintCallable, Category = "VR Base | Controller")
	void ChangeToDefaultState(bool NotifyPairedControllerIfAble = true);

	// Optional component that defines the start of hand`s aiming direction. If not overridden, will return OpenXR`s Aim Location
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "VR Base | Controller")
	USceneComponent* GetAimComponent() const;
	virtual USceneComponent* GetAimComponent_Implementation() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UDistantInteractionComponent* DistantInteractionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UGrabComponent* GrabComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetInteractionComponent* WidgetInteractionComponent;

	// ~ Object interaction

protected:

	UPROPERTY(EditDefaultsOnly, Category = "VR Motion Controller - General Setup")
	TSubclassOf<UVRControllerState> StartingState;

	UPROPERTY(BlueprintReadonly, Category = "VR Motion Controller")
	UVRControllerState* ControllerState = nullptr;
	UPROPERTY(BlueprintReadWrite, Category = "VR Motion Controller")
	FName NiagaraDefaultParameterName;

	UPROPERTY() AVRPawnBase* OwningVRPawn;
	bool bIsRightHand;

	void SetupForLocalPlayerOrSimulatedProxy();

private:
	void AttachToOwnerGrip();

	// Networking support
protected:
	// Whether this hand should be attached to a Grip and wont be moved on its own (Multiplayer and Simulated Proxy only)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VR Motion Controller - Networking Setup")
	bool bAttachToGripAsProxy;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_InitialBunchDataReceived)
	FInitialBunchData_HandController InitialBunchData_HandController;
	UFUNCTION() void OnRep_InitialBunchDataReceived();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
