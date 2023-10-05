#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SBuildData.h"
#include "SBuildItemDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class ORTHOPVPSTRATEGY_API USBuildItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Data Settings")
	FPrimaryAssetType DataType;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(DataType, GetFName());
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
	TEnumAsByte<EBuildAssetFilter> BuildAssetFilter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	FText TitleText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	TSoftObjectPtr<UTexture2D> Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Classes")
	TSoftClassPtr<AActor> BuildClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Classes")
	TSoftClassPtr<AActor> BuiltClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
	TArray<TSoftObjectPtr<UStaticMesh>> BuildMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	TSoftObjectPtr<UStaticMesh> BuildingMesh_Complete;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selection")
	TSoftObjectPtr<UMaterialInstance> PlaceMaterial;
};
