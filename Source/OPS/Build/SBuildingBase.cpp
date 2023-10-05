// Fill out your copyright notice in the Description page of Project Settings.


#include "SBuildingBase.h"
#include "OPS/Data/SBuildItemDataAsset.h"

ASBuildingBase::ASBuildingBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
}


void ASBuildingBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASBuildingBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASBuildingBase::SetData(USBuildItemDataAsset* BuildingData)
{
	if (BuildingData)
	{
		Data = BuildingData;
	}

	if (!Mesh || !Data) return;

	if (UStaticMesh* CompletedMesh = Data->BuildingMesh_Complete.LoadSynchronous())
	{
		Mesh->SetStaticMesh(CompletedMesh);
	}
}
