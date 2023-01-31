// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "General/Structs/MultiplayerStructs.h"

#include "VRBaseDataReplicationComponent.generated.h"

/**
 * Handles replication of base player data, such as HMD height and controllers
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), hidecategories = ("Variable", "Tags", "ComponentTick", "ComponentReplication", "Activation", "Cooking", "AssetUserData", "Collision", "Sockets", "Events"))
class VRBASE_API UVRBaseDataReplicationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UVRBaseDataReplicationComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual bool GetLocalDataToReplicate(float DeltaTime, FVRBasePlayerData& DataRef);
	void SetNewVRData(FVRBasePlayerData& NewVRData);

protected:
	// Default logic implementation does not rotate simulated proxies so data that is sent and received needs to be in the world space
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Customizable")
	bool ConvertDataToWorldSpace;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default Values")
	FTransform Transform_Hand_Right;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default Values")
	FTransform Transform_Hand_Left;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default Values")
	FTransform Transform_HMD;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay variables")
	bool DetachableCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Network variables")
	float MaxServerRPCsPerSecond;
	// True to send hand locations as grip locations (works fine for kinematic hands), use false if hands are physical and their location might differ from grip
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Network variables")
	bool ReportGripLocationForHands;

	class AVRPawnBase* OwnerVRPawn;

	FORCEINLINE void LerpRelativeTransform(USceneComponent* Comp, const FTransform& Start, const FTransform& End, float LerpValue)
	{
		FTransform NewTransform = UKismetMathLibrary::TLerp(Start, End, LerpValue);
		Comp->SetRelativeTransform(NewTransform);
	};

	float LastTransformsUpdateTime;
	bool bShouldLerp;
	FTransform LastTransform_Hand_Left;
	FTransform LastTransform_Hand_Right;
	FTransform LastTransform_HMD;
};
