// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "General/Structs/MultiplayerStructs.h"
#include "VRBaseMultiplayerSettings.generated.h"

/**
 * 
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Multiplayer Settings"))
class VRBASEMULTIPLAYER_API UVRBaseMultiplayerSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Online Session - General", meta = (ConfigRestartRequired = true), meta = (ToolTip = "Set to false if using another Subsystem class"))
	bool bCreateMainOnlineSessionSubsystem = true;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Online Session - General", meta = (ToolTip = "Call IdentityInterface->AutoLogin(0) on init"))
	bool bAutoLogin = false;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Online Session - General", meta = (ToolTip = "IsLan boolean when creating sessions"))
	bool bUseLanForEditorBuilds = true;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Online Session - General", meta = (ToolTip = "Using in a shipping build is redundant. Used to filter out unrelated sessions (For example when using 480 steam app id)."))
	FName CustomSessionQueryKey = FName(NAME_None);

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Online Session - General", meta = (ToolTip = "Some specific sessions handling depends on a current subsystem used"))
	EOnlineSubsystemType SubsystemType = EOnlineSubsystemType::Null;

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Online Session - Cosmetic")
	bool bShowLoadingScreenOnLevelChange = false;

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Online Session - EOS", meta = (ToolTip = "Auto manage voice chat when hosting or joining the game"))
	bool bUseEOSVoiceChat = false;
};
