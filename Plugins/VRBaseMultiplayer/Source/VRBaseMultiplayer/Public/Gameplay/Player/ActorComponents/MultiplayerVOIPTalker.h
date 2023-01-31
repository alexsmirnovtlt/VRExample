// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Net/VoiceConfig.h"
#include "MultiplayerVOIPTalker.generated.h"

/**
 * 
 */
UCLASS()
class VRBASEMULTIPLAYER_API UMultiplayerVOIPTalker : public UVOIPTalker
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay();
	virtual void OnTalkingBegin(UAudioComponent* AudioComponent) override;
	virtual void OnTalkingEnd() override;

	UFUNCTION(BlueprintCallable, Category = "Audio|Voice|Notification")
	bool IsTalking() const { return bIsNowTalking; };

protected:
	UPROPERTY()
	AActor* ValidOwner;
	bool bIsNowTalking;
};
