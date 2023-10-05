// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Interfaces/Selectable.h"
#include "AI/AIData.h"
#include "OrthoPvpStrategyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class USCharacterData;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);


UCLASS(config=Game)
class AOrthoPvpStrategyCharacter : public ACharacter, public ISelectable
{	
	GENERATED_BODY()
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:
	AOrthoPvpStrategyCharacter();
	
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	FTransform GetPositionTransform(const FVector Position) const;

public:

	/** ISelectable Interface */

	virtual void Select() override;
	virtual void DeSelect() override;
	virtual void Highlight(const bool Highlight, const EHighlightType HighlightType) override;

	/** ISelectable Ends */

	UPROPERTY()
	bool bSelected;

	UPROPERTY();
	bool bMouseOver;

	/** Command */
	UFUNCTION()
	void CommandMoveToLocation(const FCommandData CommandData);
	UFUNCTION()
	void SetAIController(ASAIController* NewAIController) { SAIController = NewAIController; }

	UFUNCTION()
	void SetToMoveMarker(const FVector Location);

protected:
	/** Data Management */
	UFUNCTION()
	void LoadData() const;

	UFUNCTION()
	USCharacterData* GetCharacterData() const;

	UFUNCTION()
	UMaterialInstance* GetHighlightMaterial() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Data, meta = (AllowedTypes = "CharacterData"), Replicated)
	FPrimaryAssetId CharacterDataAssetId;

	/** End Data Management*/

	UFUNCTION()
	void CommandMove(const FCommandData CommandData);

	UFUNCTION()
	void DestinationReached(const FCommandData CommandData);
	UFUNCTION()
	void SetWalk() const;
	UFUNCTION()
	void SetRun() const;
	UFUNCTION()
	void SetSprint() const;
	UFUNCTION()
	void SetOrientation(float DeltaTime);
	UFUNCTION()
	bool IsOrientated() const;
	UFUNCTION()
	void SetMoveMarker(const FVector Location);

	UPROPERTY()
	UCharacterMovementComponent* CharacterMovementComp;

	UPROPERTY()
	float MaxSpeed;

	UPROPERTY()
	FRotator TargetOrientation;

	UPROPERTY()
	bool bShouldOrientate;

	UPROPERTY()
	class ASAIController* SAIController;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI Settings")
	TSubclassOf<AActor> MoveMarkerClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI Settings")
	TSubclassOf<AActor> ToMoveMarkerClass;

	UPROPERTY()
	AActor* MoveMarker;

	UPROPERTY()
	AActor* ToMoveMarker;

	/** End Command */
};

