#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OPS/AI/AIData.h"
#include "InputMappingContext.h"
#include "SPlayerController.generated.h"

class UAssetManager;
class UFormationDataAsset;
class USBuildComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectedUpdatedDelegate);

UCLASS()
class ORTHOPVPSTRATEGY_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ASPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Handle_Selection(AActor* ActorToSelect);
	void Handle_Selection(TArray<AActor*> ActorsToSelect);

	UFUNCTION()
	void Handle_DeSelection(AActor* ActorToDeSelect);
	void Handle_DeSelection(TArray<AActor*> ActorsToDeSelect);

	UFUNCTION()
	FVector GetMousePositionOnGround() const;
	UFUNCTION()
	FVector GetMousePositionOnVisibility() const;

	UFUNCTION()
	void Handle_MouseOver(AActor* ActorToMouseOver, bool Highlight);
	
	UFUNCTION()
	bool HasGroupSelection() const { return Selected.Num() > 1; }

	UPROPERTY()
	FSelectedUpdatedDelegate OnSelectedUpdated;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	bool ActorSelected(AActor* ActorToCheck) const;

	UFUNCTION(Server, Reliable)
	void Server_Select(AActor* ActorToSelect);

	UFUNCTION(Server, Reliable)
	void Server_Select_Group(const TArray<AActor*>& ActorsToSelect);

	UFUNCTION(Server, Reliable)
	void Server_DeSelect(AActor* ActorToDeSelect);

	UFUNCTION(Server, Reliable)
	void Server_DeSelect_Group(const TArray<AActor*>& ActorsToDeSelect);

	UFUNCTION(Server, Reliable)
	void Server_ClearSelected();

	UFUNCTION()
	void OnRep_Selected();

	UPROPERTY(ReplicatedUsing = OnRep_Selected)
	TArray<AActor*> Selected;

	/** Command Functions */
public:
	UFUNCTION()
	void DisplaySuggestedCommand(FCommandData CommandData);

	UFUNCTION()
	void CommandSelected(FCommandData CommandData);

	UFUNCTION()
	void CreateFormationData();

	UFUNCTION()
	void OnFormationDataLoaded(TArray<FPrimaryAssetId> Formations);

	UFUNCTION()
	UFormationDataAsset* GetFormationData();

	UFUNCTION()
	void CalculateOffset(const int Index, FCommandData& CommandData);

	UFUNCTION()
	void UpdateFormation(const EFormation Formation);

	UFUNCTION()
	void UpdateSpacing(const float NewFormationSpacing);

protected:
	UFUNCTION(Server, Reliable)
	void Server_CommandSelected(FCommandData CommandData);

	UPROPERTY()
	TEnumAsByte<EFormation> CurrentFormation;

	UPROPERTY()
	float FormationSpacing;

	UPROPERTY();
	UAssetManager* AssetManager;

	UPROPERTY()
	TArray<UFormationDataAsset*> FormationData;

	/** End Command Functions*/
public:
	/** Enchanced Input*/
	UFUNCTION()
	void AddInputMapping(const UInputMappingContext* InputMapping, const int32 MappingPriority = 0) const;

	UFUNCTION()
	void RemoveInputMapping(const UInputMappingContext* InputMapping) const;

	UFUNCTION()
	void SetInputDefault(const bool Enabled = true) const;	
	
	UFUNCTION()
	void SetInputPlacement(const bool Enabled = true) const;

	UFUNCTION()
	void SetInputBuildMode(const bool Enabled = true) const;

	UFUNCTION()
	void SetInputShift(const bool Enabled = true) const;

	UFUNCTION()
	void SetInputAlt(const bool Enabled = true) const;

	UFUNCTION()
	void SetInputCtrl(const bool Enabled = true) const;

	UFUNCTION()
	UDataAsset* GetInputActionsAsset() const { return PlayerActionsAsset; }

protected:
	virtual void SetupInputComponent() override;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Player Settings")
	UDataAsset* PlayerActionsAsset;

	/** Placement */
public:
	UFUNCTION()
	bool IsPlacementModeEnabled() const { return bPlacementModeEnabled; }

	UFUNCTION()
	void SetPlacementPreview();

	UFUNCTION()
	void Place();

	UFUNCTION()
	void PlaceCancel();

	TEnumAsByte<EBuildAssetFilter> GetBuildFilter() const { return BuildAssetFilter; }

protected:
	UFUNCTION()
	void UpdatePlacement() const;

	UFUNCTION(Server, Reliable)
	void Server_Place(AActor* PlacementPreviewToSpawn);

	UFUNCTION(Client, Reliable)
	void EndPlacement();

	UPROPERTY()
	bool bPlacementModeEnabled;

	UPROPERTY()
	AActor* PlacementPreviewActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Placeable")
	TSubclassOf<AActor> PreviewActorType;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USBuildComponent* BuildComponent;

	UPROPERTY()
	TEnumAsByte<EBuildAssetFilter> BuildAssetFilter = EBuildAssetFilter::Filter1;
};
