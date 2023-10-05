#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OPS/AI/AIData.h"
#include "FormationDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class ORTHOPVPSTRATEGY_API UFormationDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data Settings")
	FPrimaryAssetType DataType;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(DataType, GetFName());
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TEnumAsByte<EFormation> FormationType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FVector Offset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool Alternate;
};
