#include "PlacementPreview.h"

APlacementPreview::APlacementPreview()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComponent;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	bReplicates = false;
	SetActorEnableCollision(false);
}

void APlacementPreview::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlacementPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

