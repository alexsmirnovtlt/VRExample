// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ConstrainedMovable.generated.h"

USTRUCT(BlueprintType)
struct VRBASE_API FConstrainedAxis
{
	GENERATED_BODY()

	FConstrainedAxis()
	{
		bConstrain = false;
		bFreeze = false;
		MinValue = 0.f;
		MaxValue = 0.f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bConstrain;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFreeze;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxValue;
};

USTRUCT(BlueprintType)
struct VRBASE_API FConstrainedActorValues
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FConstrainedAxis Axis_X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FConstrainedAxis Axis_Y;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FConstrainedAxis Axis_Z;
};

UINTERFACE(MinimalAPI, BlueprintType)
class UConstrainedMovable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VRBASE_API IConstrainedMovable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | ConstrainedMovable")
	bool SetNewLocationConstrained(FVector DesiredLocation, bool bSweep);
};
