// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "OPS/AI/AIData.h"
#include "PlayerPawn.generated.h"

class USBuildComponent;

UCLASS()
class ORTHOPVPSTRATEGY_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	APlayerPawn();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void GetTerrainPosition(FVector& TerrainPosition) const;

	UFUNCTION()
	void EdgeScroll();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float MoveSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float EdgeScrollSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float RotateSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float RotatePitchMin;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float RotatePitchMax;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float ZoomSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float MinZoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float MaxZoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float Smoothing;

	UFUNCTION()
	AActor* GetSelectedObject();

	UFUNCTION()
	void CreateSelectionBox();

	UPROPERTY()
	USBuildComponent* BuildComponent;

	UPROPERTY()
	class ASPlayerController* SPlayerController;

	UPROPERTY()
	AActor* MouseOverActor;

	UPROPERTY()
	AActor* LastFrameMouseOverActor;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Mouse")
	TSubclassOf<class ASelectionBox> SelectionBoxClass;

	UPROPERTY()
	ASelectionBox* SelectionBox;

	UPROPERTY()
	bool BoxSelect;

	UPROPERTY()
	FVector SelectHitLocation;

	/** Command */
	UFUNCTION()
	FCommandData CreateCommandData(const ECommandType Type) const;

	UPROPERTY()
	FVector CommandLocation;

	UPROPERTY()
	bool bIsCommanding;

	/** End Command*/

private:
	void CameraBounds();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	USceneComponent* SceneComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class UCameraComponent* CameraComponent;

	UPROPERTY()
	FVector TargetLocation;
	UPROPERTY()
	FRotator TargetRotation;
	UPROPERTY()
	float TargetZoom;

	/** Enchanced Input*/
	/** Default */
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void Rotate(const FInputActionValue& Value);

	UFUNCTION()
	void Zoom(const FInputActionValue& Value);

	UFUNCTION()
	void Select(const FInputActionValue& Value);

	UFUNCTION()
	void SelectHold(const FInputActionValue& Value);

	UFUNCTION()
	void SelectEnd(const FInputActionValue& Value);

	UFUNCTION()
	void SelectDoubleTap(const FInputActionValue& Value);

	UFUNCTION()
	void TestPlacement(const FInputActionValue& Value);

	UFUNCTION()
	void CommandStart(const FInputActionValue& Value);

	UFUNCTION()
	void Command(const FInputActionValue& Value);

	/** Modifier */

	UFUNCTION()
	void Shift(const FInputActionValue& Value);

	UFUNCTION()
	void Alt(const FInputActionValue& Value);
	UFUNCTION()
	void Ctrl(const FInputActionValue& Value);

	/** Placement */

	UFUNCTION()
	void Place(const FInputActionValue& Value);

	UFUNCTION()
	void PlaceCancel(const FInputActionValue& Value);

	/** BuildMode */

	UFUNCTION()
	void BuildDeploy(const FInputActionValue& Value);

	UFUNCTION()
	void BuildCancel(const FInputActionValue& Value);

	/** Shift */
	UFUNCTION()
	void ShiftSelect(const FInputActionValue& Value);

	UFUNCTION()
	void ShiftCommand(const FInputActionValue& Value);

	/** Alt */
	UFUNCTION()
	void AltSelect(const FInputActionValue& Value);

	UFUNCTION()
	void AltSelectEnd(const FInputActionValue& Value);

	UFUNCTION()
	void AltCommand(const FInputActionValue& Value);

	/** Ctrl */
	UFUNCTION()
	void CtrlSelect(const FInputActionValue& Value);

	UFUNCTION()
	void CtrlSelectEnd(const FInputActionValue& Value);

	UFUNCTION()
	void CtrlCommand(const FInputActionValue& Value);
};
