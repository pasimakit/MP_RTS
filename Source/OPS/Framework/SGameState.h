// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "OPS/Data/SBuildData.h"
#include "SGameState.generated.h"

/**
 * 
 */
UCLASS()
class ORTHOPVPSTRATEGY_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ASGameState();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	void AddPlacedObject(const FWorldSelectableData NewObject);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_AddPlacedObject(const FWorldSelectableData NewObject);

	UPROPERTY(Replicated)
	TArray<FWorldSelectableData> PlacedObjects;
	
};
