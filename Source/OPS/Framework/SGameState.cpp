// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameState.h"
#include "Net/UnrealNetwork.h"

ASGameState::ASGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
}

void ASGameState::BeginPlay()
{
	Super::BeginPlay();
}



void ASGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameState, PlacedObjects);
}

void ASGameState::AddPlacedObject(const FWorldSelectableData NewObject)
{
	Server_AddPlacedObject(NewObject);

}
void ASGameState::Server_AddPlacedObject_Implementation(const FWorldSelectableData NewObject)
{
	if (HasAuthority())
	{
		PlacedObjects.Add(NewObject);
	}
}
