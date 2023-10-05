// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SCharacterData.generated.h"

/**
 * 
 */
UCLASS()
class ORTHOPVPSTRATEGY_API USCharacterData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	/** AssetManager Asset Type of this item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Asset Settings")
	FPrimaryAssetType DataType;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(DataType, GetFName());
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selection")
	TSoftObjectPtr<UMaterialInstance> HighlightMaterial;
};
