// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OPS/Data/SBuildData.h"
#include "SBuildComponent.generated.h"

class ASPlayerController;
class UAssetManager;
class USBuildItemDataAsset;
class ASBuildable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuildModeEnterDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ORTHOPVPSTRATEGY_API USBuildComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USBuildComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	static USBuildComponent* FindBuildComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<USBuildComponent>() : nullptr); }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	UFUNCTION()
	void EnterBuildPlacementMode(USBuildItemDataAsset* BuildItemData);
	UFUNCTION()
	void LoadBuildData();
	UFUNCTION()
	void ExitBuildMode();
	UFUNCTION()
	void BuildDeploy();
	TArray<FPrimaryAssetId> GetBuildData() const { return BuildItemsData; }
	FOnBuildModeEnterDelegate OnBuildModeEnterEvent;
protected:
	virtual void BeginPlay() override;
	void UpdatePlacementStatus();

	UFUNCTION()
	void OnBuildDataLoaded(TArray<FPrimaryAssetId> BuildAssetsIds);

	UFUNCTION(Client, Reliable)
	void Client_EnterBuildPlacementMode(USBuildItemDataAsset* BuildItemData);

	UFUNCTION(Server, Reliable)
	void Server_BuildDeploy(USBuildItemDataAsset* BuildData, const FTransform& Location);

	UFUNCTION(Server, Reliable)
	void Server_BuildComplete(USBuildItemDataAsset* BuildData, const FTransform& Location);
	UFUNCTION()
	void OnBuildComplete(const TEnumAsByte<EBuildState> BuildState);

	UPROPERTY()
	UAssetManager* AssetManager;

	UPROPERTY(Replicated)
	TArray<FPrimaryAssetId> BuildItemsData;

	UPROPERTY()
	ASPlayerController* SPlayerController;

	UPROPERTY()
	AActor* OwningActor;

	UPROPERTY()
	UWorld* WorldContext;

	UPROPERTY(Replicated)
	ASBuildable* BuildObject;

	UPROPERTY()
	ASBuildable* Client_BuildObject;

	UPROPERTY()
	bool bIsPlaceable = false;
};
