// Fill out your copyright notice in the Description page of Project Settings.


#include "SBuildable.h"
#include "OPS/Data/SBuildItemDataAsset.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"


ASBuildable::ASBuildable()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = BoxCollider;
	BoxCollider->SetCollisionProfileName("OverlapAll");
	BoxCollider->SetGenerateOverlapEvents(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionProfileName("OverlapAll");
}

void ASBuildable::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASBuildable::Init(USBuildItemDataAsset* BuildItemData, const TEnumAsByte<EBuildState> NewState)
{
	if (!BuildItemData) return;

	BuildState = NewState;
	Data = BuildItemData;

	if (BuildState == EBuildState::Building)
	{
		StartBuild();
	}
	else
	{
		InitBuildPreview();
	}
}

void ASBuildable::UpdateOverlayMaterial(const bool bCanPlace) const
{
	DynamicOverlayMaterial->SetScalarParameterValue(TEXT("Status"), bCanPlace ? 1.f : 0.f);
}

void ASBuildable::InitBuildPreview()
{
	if (!Mesh || !Data || !BoxCollider) return;

	if (UStaticMesh* PreviewMesh = Data->BuildingMesh_Complete.LoadSynchronous())
	{
		Mesh->SetStaticMesh(PreviewMesh);
		UpdateCollider();
		SetOverlayMaterial();
	}
}

void ASBuildable::StartBuild()
{
	// Initialize build state values
	BuildProgression = 0.f;
	UpdateBuildProgressionMesh();

	// Start build timer
	GetWorldTimerManager().SetTimer(Handle_BuildTimer, this, &ASBuildable::UpdateBuildProgression, 2.f, true, 2.f);

	// Update state
	BuildState = EBuildState::Building;
}

void ASBuildable::EndBuild()
{
	// Stop build timer
	GetWorldTimerManager().ClearTimer(Handle_BuildTimer);

	// Check if building was aborted
	if (BuildState != EBuildState::BuildComplete)
	{
		BuildState = EBuildState::BuildAborted;
	}

	OnBuildCompleteEvent.Broadcast(BuildState);
}

void ASBuildable::UpdateCollider()
{
	if (!Mesh && !BoxCollider) return;

	FVector MinMeshBounds, MaxMeshBounds;
	Mesh->GetLocalBounds(MinMeshBounds, MaxMeshBounds);

	BoxCollider->SetBoxExtent(FVector(
		FMath::Max(FMath::RoundToInt(MaxMeshBounds.X + 10.f), 100.f),
		FMath::Max(FMath::RoundToInt(MaxMeshBounds.Y + 10.f), 100.f),
		FMath::Max(FMath::RoundToInt(MaxMeshBounds.Z + 10.f), 100.f)
	),	true );

	BoxCollider->SetWorldRotation(UKismetMathLibrary::MakeRotFromX(FVector(1.f, 0.f, 0.f)));
}

void ASBuildable::SetOverlayMaterial()
{
	if (!Data) return;

	const FSoftObjectPath AssetPath = Data->PlaceMaterial.ToSoftObjectPath();
	if (UMaterialInstance* OverlayMaterial = Cast<UMaterialInstance>(AssetPath.TryLoad()))
	{
		DynamicOverlayMaterial = UMaterialInstanceDynamic::Create(OverlayMaterial, this);
		if (DynamicOverlayMaterial)
		{
			TArray<UStaticMeshComponent*> Components;
			GetComponents<UStaticMeshComponent>(Components);
			for (int32 i = 0; i < Components.Num(); i++)
			{
				Components[i]->SetOverlayMaterial(DynamicOverlayMaterial);
			}
		}
	}
}

void ASBuildable::UpdateBuildProgressionMesh()
{
	if (!Data || !Mesh) return;

	const int32 BuildMeshIndex = FMath::FloorToInt(BuildProgression * Data->BuildMeshes.Num());
	if (Data->BuildMeshes.IsValidIndex(BuildMeshIndex))
	{
		if (UStaticMesh* DisplayMesh = Data->BuildMeshes[BuildMeshIndex].LoadSynchronous())
		{
			Mesh->SetStaticMesh(DisplayMesh);
		}
	}
}

void ASBuildable::UpdateBuildProgression()
{
	BuildProgression += 1.f / Data->BuildMeshes.Num();

	// Check if building is completed
	if (BuildProgression >= 1.f)
	{
		if (UStaticMesh* DisplayMesh = Data->BuildingMesh_Complete.LoadSynchronous())
		{
			Mesh->SetStaticMesh(DisplayMesh);
		}
		BuildState = EBuildState::BuildComplete;
		EndBuild();
	}
	else
	{
		UpdateBuildProgressionMesh();
	}
}

