// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OPS/Data/SBuildData.h"
#include "SBuildable.generated.h"

class UAssetManager;
class USBuildItemDataAsset;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildCompleteEvent, const TEnumAsByte<EBuildState>, BuildState);

UCLASS()
class ORTHOPVPSTRATEGY_API ASBuildable : public AActor
{
	GENERATED_BODY()
	
public:	
	ASBuildable();
	void Init(USBuildItemDataAsset* BuildItemData, const TEnumAsByte<EBuildState> NewState = EBuildState::NoBuild);
	USBuildItemDataAsset* GetData() const { return Data; }
	void UpdateOverlayMaterial(const bool bCanPlace = true) const;

	FOnBuildCompleteEvent OnBuildCompleteEvent;

protected:
	virtual void BeginPlay() override;
	void InitBuildPreview();
	void StartBuild();
	void EndBuild();
	void UpdateCollider();
	void SetOverlayMaterial();
	void UpdateBuildProgressionMesh();
	void UpdateBuildProgression();

	UPROPERTY()
	UAssetManager* AssetManager;

	UPROPERTY()
	USBuildItemDataAsset* Data;

	UPROPERTY()
	float BuildProgression = 0.f;

	UPROPERTY()
	FTimerHandle Handle_BuildTimer;

	UPROPERTY()
	TEnumAsByte<EBuildState> BuildState = EBuildState::NoBuild;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicOverlayMaterial;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;
};
