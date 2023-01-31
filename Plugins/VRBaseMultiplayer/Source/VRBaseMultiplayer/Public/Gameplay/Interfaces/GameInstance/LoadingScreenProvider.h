// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LoadingScreenProvider.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class ULoadingScreenProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VRBASEMULTIPLAYER_API ILoadingScreenProvider
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | LoadingScreenProvider")
	void LevelLoadingScreen_Show();
	virtual void LevelLoadingScreen_Show_Implementation() {};
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | LoadingScreenProvider")
	void LevelLoadingScreen_Hide();
	virtual void LevelLoadingScreen_Hide_Implementation() {};
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | LoadingScreenProvider")
	void ShowLevelLoadingScreenOnLevelChange(bool bShow);
	virtual void ShowLevelLoadingScreenOnLevelChange_Implementation(bool bShow) {};
};
