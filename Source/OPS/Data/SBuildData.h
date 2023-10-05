#pragma once


#include "SBuildData.generated.h"

UENUM()
enum EBuildAssetFilter
{
	Filter1,
	Filter2
};

UENUM()
enum EBuildState
{
	NoBuild,
	Building,
	BuildAborted,
	BuildComplete
};

USTRUCT()
struct FWorldSelectableData
{
	GENERATED_BODY()

public:
	FWorldSelectableData(): Object(nullptr), OwningPlayer(nullptr) {}
	FWorldSelectableData(AActor* InObject, APlayerController* SPlayerController): Object(InObject), OwningPlayer(SPlayerController) {}

	UPROPERTY()
	AActor* Object;

	UPROPERTY()
	APlayerController* OwningPlayer;
};
