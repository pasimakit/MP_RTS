// Fill out your copyright notice in the Description page of Project Settings.


#include "SBuildComponent.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "OPS/Data/SBuildItemDataAsset.h"
#include "OPS/Controller/SPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "OPS/Build/SBuildable.h"
#include "OPS/Framework/SGameState.h"
#include "SBuildingBase.h"

USBuildComponent::USBuildComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void USBuildComponent::BeginPlay()
{
	Super::BeginPlay();
	
	verify((OwningActor = GetOwner()) != nullptr);
	verify((WorldContext = OwningActor->GetWorld()) != nullptr);
	verify((SPlayerController = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))) != nullptr);
}

void USBuildComponent::UpdatePlacementStatus()
{
	if (!Client_BuildObject) return;

	TArray<AActor*> OverlappingActors;
	Client_BuildObject->GetOverlappingActors(OverlappingActors);
	bIsPlaceable = OverlappingActors.Num() <= 0;
	Client_BuildObject->UpdateOverlayMaterial(bIsPlaceable);
}

void USBuildComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Client_BuildObject || !SPlayerController) return;

	const FVector MouseLocationOnGround = SPlayerController->GetMousePositionOnGround();

	if (Client_BuildObject->GetActorLocation() != MouseLocationOnGround)
	{
		Client_BuildObject->SetActorLocation(MouseLocationOnGround);
	}

	UpdatePlacementStatus();
}

void USBuildComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USBuildComponent, BuildItemsData);
	DOREPLIFETIME(USBuildComponent, BuildObject);
}

void USBuildComponent::EnterBuildPlacementMode(USBuildItemDataAsset* BuildItemData)
{
	if (!SPlayerController) return;

	SPlayerController->SetInputBuildMode(true);

	Client_EnterBuildPlacementMode(BuildItemData);
}

void USBuildComponent::LoadBuildData()
{
	verify((AssetManager = UAssetManager::GetIfInitialized()) != nullptr);

	const FPrimaryAssetType AssetType("BuildData");
	TArray<FPrimaryAssetId> BuildDataAssets;
	AssetManager->GetPrimaryAssetIdList(AssetType, BuildDataAssets);

	if (BuildDataAssets.Num() > 0)
	{
		const TArray<FName> Bundles;
		const FStreamableDelegate FormationDataLoadedDelegate = FStreamableDelegate::CreateUObject(this, &USBuildComponent::OnBuildDataLoaded, BuildDataAssets);
		AssetManager->LoadPrimaryAssets(BuildDataAssets, Bundles, FormationDataLoadedDelegate);
	}
}

void USBuildComponent::ExitBuildMode()
{
	if (!SPlayerController) return;

	SPlayerController->SetInputBuildMode(false);

	if (Client_BuildObject)
	{
		Client_BuildObject->Destroy();
	}
}

void USBuildComponent::BuildDeploy()
{
	if (!Client_BuildObject) return;

	if (bIsPlaceable)
	{
		Server_BuildDeploy(Client_BuildObject->GetData(), Client_BuildObject->GetActorTransform());
		ExitBuildMode();
	}
	else
	{
		// cannot build here notify
	}

}

void USBuildComponent::OnBuildDataLoaded(TArray<FPrimaryAssetId> BuildAssetsIds)
{
	if (!SPlayerController) return;

	for (int i = 0; i < BuildAssetsIds.Num(); ++i)
	{
		if (const USBuildItemDataAsset* BuildDataAsset = Cast<USBuildItemDataAsset>(AssetManager->GetPrimaryAssetObject(BuildAssetsIds[i])))
		{
			if (BuildDataAsset->BuildAssetFilter == SPlayerController->GetBuildFilter())
			{
				BuildItemsData.Add(BuildAssetsIds[i]);
			}
		}
	}
}

void USBuildComponent::Client_EnterBuildPlacementMode_Implementation(USBuildItemDataAsset* BuildItemData)
{
	if (!SPlayerController || !BuildItemData || !WorldContext) return;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SPlayerController->GetMousePositionOnGround());
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Client_BuildObject = WorldContext->SpawnActor<ASBuildable>(BuildItemData->BuildClass.LoadSynchronous(), SpawnTransform, SpawnParams);

	if (Client_BuildObject)
	{
		Client_BuildObject->Init(BuildItemData);
	}

	OnBuildModeEnterEvent.Broadcast();
}

void USBuildComponent::OnBuildComplete(const TEnumAsByte<EBuildState> BuildState)
{
	if (!BuildObject) return;

	if (BuildState == EBuildState::BuildComplete)
	{
		Server_BuildComplete(BuildObject->GetData(), BuildObject->GetTransform());
	}
	else
	{
		BuildObject->Destroy();
	}
}

void USBuildComponent::Server_BuildComplete_Implementation(USBuildItemDataAsset* BuildData, const FTransform& Location)
{
	if (!OwningActor || !SPlayerController || !WorldContext) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (TSubclassOf<AActor> BuiltClass = BuildData->BuiltClass.LoadSynchronous())
	{
		if (AActor* BuiltObject = WorldContext->SpawnActor<AActor>(BuiltClass, Location, SpawnParams))
		{
			if (ASGameState* GameState = Cast<ASGameState>(UGameplayStatics::GetGameState(OwningActor)))
			{
				GameState->AddPlacedObject(FWorldSelectableData(BuiltObject, SPlayerController));
			}
			
			if (ASBuildingBase* Building = Cast<ASBuildingBase>(BuiltObject))
			{
				Building->SetData(BuildData);
			}
		}
	}
}

void USBuildComponent::Server_BuildDeploy_Implementation(USBuildItemDataAsset* BuildData, const FTransform& Location)
{
	if (!BuildData) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	BuildObject = WorldContext->SpawnActor<ASBuildable>(BuildData->BuildClass.LoadSynchronous(), Location, SpawnParams);
	if (BuildObject)
	{
		BuildObject->Init(BuildData, EBuildState::Building);
		BuildObject->OnBuildCompleteEvent.AddDynamic(this, &USBuildComponent::OnBuildComplete);
	}
}

