// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NonVRCharacterMovementComponent.generated.h"

/**
 * Movement component for NonVR player that adds ability to sprint 
 */
UCLASS()
class VRBASEMULTIPLAYER_API UNonVRCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	class FGDSavedMove : public FSavedMove_Character
	{
	public:

		typedef FSavedMove_Character Super;

		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;
		virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;

		uint8 SavedRequestToStartSprinting : 1;
	};

	class FNetworkPredictionDataCharacter_Client : public FNetworkPredictionData_Client_Character
	{
	public:

		FNetworkPredictionDataCharacter_Client(const UCharacterMovementComponent& ClientMovement) : FNetworkPredictionData_Client_Character(ClientMovement) {};
		virtual FSavedMovePtr AllocateNewMove() override { return FSavedMovePtr(new FGDSavedMove()); };
	};

public:
	UNonVRCharacterMovementComponent();

	virtual float GetMaxSpeed() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

public:
	UFUNCTION(BlueprintCallable, Category = Character)
	void StartSprinting() { RequestToStartSprinting = true; };
	UFUNCTION(BlueprintCallable, Category = Character)
	void StopSprinting() { RequestToStartSprinting = false; };

protected:

	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxSprintSpeed;

	uint8 RequestToStartSprinting : 1;
};

