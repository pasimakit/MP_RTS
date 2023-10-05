#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIData.h"
#include "SAIController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReachedDestinationDelegate, const FCommandData, CommandData);

UCLASS()
class ORTHOPVPSTRATEGY_API ASAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ASAIController(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void CommandMove(FCommandData CommandData);

	UPROPERTY()
	FReachedDestinationDelegate OnReachedDestination;

protected:
	virtual void OnPossess(class APawn* InPawn) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UPROPERTY()
	class AOrthoPvpStrategyCharacter* OwningCharacter;
	
	UPROPERTY()
	FCommandData CurrentCommandData;
};
