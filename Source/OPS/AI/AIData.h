#pragma once

#include "AIData.generated.h"


UENUM(BlueprintType)
enum ECommandType
{
	CommandMove,
	CommandMoveFast,
	CommandMoveSlow,
	CommandMoveAttack
};

UENUM(BlueprintType)
enum EFormation
{
	Line,
	Column,
	Wedge,
	Blob
};

USTRUCT(BlueprintType)
struct FCommandData
{
	GENERATED_BODY()

	FCommandData() : Location(FVector::ZeroVector), SourceLocation(FVector::ZeroVector), Rotation(FRotator::ZeroRotator), Type(CommandMove), Target(nullptr) {}
	FCommandData(const FVector InLocation, const FRotator InRotation, const ECommandType InType, AActor* InTarget = nullptr) :
		Location(InLocation), SourceLocation(InLocation), Rotation(InRotation), Type(InType), Target(InTarget) {}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Location;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector SourceLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rotation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<ECommandType> Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* Target;
};