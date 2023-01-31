// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VRAndNonVRGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class VRBASEMULTIPLAYER_API AVRAndNonVRGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AVRAndNonVRGameModeBase();

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Custom Parameters")
	TSubclassOf<class APawn> PawnClass_VR;
	UPROPERTY(EditDefaultsOnly, Category = "Custom Parameters")
	TSubclassOf<APawn> PawnClass_NonVR;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Parameters")
	FName VRModeOptionString;

	// Change to True for VR only projects. Editor build can still spawn NonVR pawn if default Play button was pressed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Parameters")
	bool bDefaultPawnIsVR;

private:
	bool ShouldSpawnPlayerAsVR(const FString& Options, ENetRole InRemoteRole) const;
};
