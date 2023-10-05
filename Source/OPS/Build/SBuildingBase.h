// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SBuildingBase.generated.h"

class USBuildItemDataAsset;

UCLASS()
class ORTHOPVPSTRATEGY_API ASBuildingBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASBuildingBase();
	virtual void Tick(float DeltaTime) override;
	void SetData(USBuildItemDataAsset* BuildingData);

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	USBuildItemDataAsset* Data;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;
};
