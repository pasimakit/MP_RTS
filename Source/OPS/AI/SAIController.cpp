#include "SAIController.h"
#include "OPS/OrthoPvpStrategyCharacter.h"

ASAIController::ASAIController(const FObjectInitializer& ObjectInitializer)
{
}


void ASAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	OwningCharacter = Cast<AOrthoPvpStrategyCharacter>(InPawn);
	if (OwningCharacter)
	{
		OwningCharacter->SetAIController(this);
	}
}

void ASAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	OnReachedDestination.Broadcast(CurrentCommandData);
}

void ASAIController::CommandMove(FCommandData CommandData)
{
	CurrentCommandData = CommandData;
	MoveToLocation(CommandData.Location);
}