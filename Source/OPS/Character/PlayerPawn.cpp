#include "PlayerPawn.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "OPS/Controller/SPlayerController.h"
#include "OPS/Interfaces/Selectable.h"
#include "OPS/Selection/SelectionBox.h"
#include "EnhancedInputComponent.h"
#include "OPS/Input/PlayerInputActions.h"
#include "EngineUtils.h"
#include "OPS/OrthoPvpStrategyCharacter.h"
#include "OPS/Build/SBuildComponent.h"

APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 2000.0f;
	SpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	MoveSpeed = 20.0f;
	EdgeScrollSpeed = 30.0f;
	RotateSpeed = 2.0f;
	RotatePitchMin = 10.0f;
	RotatePitchMax = 80.0f;
	ZoomSpeed = 150.0f;
	MinZoom = 500.f;
	MaxZoom = 4000.f;
	Smoothing = 2.f;
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	TargetLocation = GetActorLocation();
	TargetZoom = 3000.f;

	const FRotator Rotation = SpringArmComponent->GetRelativeRotation();
	TargetRotation = FRotator(Rotation.Pitch - 50, Rotation.Yaw, 0.0f);

	SPlayerController = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	
	if (SPlayerController)
	{
		BuildComponent = USBuildComponent::FindBuildComponent(SPlayerController);
	}

	CreateSelectionBox();
}

void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraBounds();
	EdgeScroll();
	MouseOverActor = GetSelectedObject();
	if (!BoxSelect)
	{
		if (LastFrameMouseOverActor && LastFrameMouseOverActor != MouseOverActor)
		{
			SPlayerController->Handle_MouseOver(LastFrameMouseOverActor, false);
		}
		else if (MouseOverActor)
		{
			SPlayerController->Handle_MouseOver(MouseOverActor, true);
		}
	}
	LastFrameMouseOverActor = MouseOverActor;

	const FVector InterpolatedLocation = UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, Smoothing);
	SetActorLocation(InterpolatedLocation);

	const float InterpolatedZoom = UKismetMathLibrary::FInterpTo(SpringArmComponent->TargetArmLength, TargetZoom, DeltaTime, Smoothing);
	SpringArmComponent->TargetArmLength = InterpolatedZoom;

	const FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(SpringArmComponent->GetRelativeRotation(), TargetRotation, DeltaTime, Smoothing);
	SpringArmComponent->SetRelativeRotation(InterpolatedRotation);

	if (bIsCommanding)
	{
		FCommandData Command= CreateCommandData(ECommandType::CommandMove);
		Command.SourceLocation = CommandLocation;

		SPlayerController->DisplaySuggestedCommand(Command);
	}
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	const ASPlayerController* PlayerController = Cast<ASPlayerController>(GetController());

	if (IsValid(Input) && IsValid(PlayerController))
	{
		if(const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerController->GetInputActionsAsset()))
		{
			/** Default */
			EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->Move, this, &APlayerPawn::Move);
			EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->Look, this, &APlayerPawn::Look);
			EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->Zoom, this, &APlayerPawn::Zoom);
			EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->Rotate, this, &APlayerPawn::Rotate);
			EPlayerInputActions::BindInput_StartTriggerComplete(Input, PlayerActions->Select, this, &APlayerPawn::Select, &APlayerPawn::SelectHold, &APlayerPawn::SelectEnd);
			EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->TestPlacement, this, &APlayerPawn::TestPlacement);
			EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->SelectDoubleTap, this, &APlayerPawn::SelectDoubleTap);
			EPlayerInputActions::BindInput_Simple(Input, PlayerActions->Command, this, &APlayerPawn::CommandStart, &APlayerPawn::Command);

			/** Placement */
			EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->Place, this, &APlayerPawn::Place);
			EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->PlaceCancel, this, &APlayerPawn::PlaceCancel);

			/** Placement */
			EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->BuildDeploy, this, &APlayerPawn::BuildDeploy);
			EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->BuildCancel, this, &APlayerPawn::BuildCancel);

			/** Shift */
			EPlayerInputActions::BindInput_Simple(Input, PlayerActions->Shift, this, &APlayerPawn::Shift, &APlayerPawn::Shift);
			EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->ShiftSelect, this, &APlayerPawn::ShiftSelect);
			EPlayerInputActions::BindInput_Simple(Input, PlayerActions->ShiftCommand, this, &APlayerPawn::CommandStart, &APlayerPawn::ShiftCommand);

			/** Alt */
			EPlayerInputActions::BindInput_Simple(Input, PlayerActions->Alt, this, &APlayerPawn::Alt, &APlayerPawn::Alt);
			EPlayerInputActions::BindInput_StartTriggerComplete(Input, PlayerActions->AltSelect, this, &APlayerPawn::AltSelect, &APlayerPawn::SelectHold, &APlayerPawn::AltSelectEnd);
			EPlayerInputActions::BindInput_Simple(Input, PlayerActions->AltCommand, this, &APlayerPawn::CommandStart, &APlayerPawn::AltCommand);

			/** Ctrl */
			EPlayerInputActions::BindInput_Simple(Input, PlayerActions->Ctrl, this, &APlayerPawn::Ctrl, &APlayerPawn::Ctrl);
			EPlayerInputActions::BindInput_StartTriggerComplete(Input, PlayerActions->CtrlSelect, this, &APlayerPawn::CtrlSelect, &APlayerPawn::SelectHold, &APlayerPawn::CtrlSelectEnd);
			EPlayerInputActions::BindInput_Simple(Input, PlayerActions->CtrlCommand, this, &APlayerPawn::CommandStart, &APlayerPawn::CtrlCommand);
		}
	}
}


void APlayerPawn::CreateSelectionBox()
{
	if (!SelectionBoxClass) return;

	if (UWorld* WorldContext = GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = this;
		SpawnParams.Owner = this;
		SelectionBox = WorldContext->SpawnActor<ASelectionBox>(SelectionBoxClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (SelectionBox)
		{
			SelectionBox->SetOwner(this);
		}
	}
}

void APlayerPawn::EdgeScroll()
{
	if (UWorld* WorldContext = GetWorld())
	{
		FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(WorldContext);
		const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(WorldContext);
		MousePosition = MousePosition * UWidgetLayoutLibrary::GetViewportScale(WorldContext);
		MousePosition.X = MousePosition.X / ViewportSize.X;
		MousePosition.Y = MousePosition.Y / ViewportSize.Y;

		// Calculate mouse position for scrolling
		if (MousePosition.X > 0.98f && MousePosition.X < 1.f) 
		{
			TargetLocation += SpringArmComponent->GetTargetRotation().RotateVector(FVector(0.f, EdgeScrollSpeed, 0.f));
		}
		else if (MousePosition.X < 0.02f && MousePosition.X < 0.f) 
		{
			TargetLocation += SpringArmComponent->GetTargetRotation().RotateVector(FVector(0.f, -EdgeScrollSpeed, 0.f));
		}

		if (MousePosition.Y > 0.98f && MousePosition.Y < 1.0f) 
		{
			TargetLocation += SpringArmComponent->GetTargetRotation().RotateVector(FVector(-EdgeScrollSpeed, 0.f, 0.f));
		}
		else if (MousePosition.Y < 0.02f && MousePosition.Y < .0f) 
		{
			TargetLocation += SpringArmComponent->GetTargetRotation().RotateVector(FVector(EdgeScrollSpeed, 0.f, 0.f));
		}

		GetTerrainPosition(TargetLocation);
	}
}

AActor* APlayerPawn::GetSelectedObject()
{
	if (UWorld* World = GetWorld())
	{
		FVector WorldLocation, WorldDirection;
		SPlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
		FVector End = WorldDirection * 1000000.f + WorldLocation;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		FHitResult Hit;
		if (World->LineTraceSingleByChannel(Hit, WorldLocation, End, ECC_Visibility, Params))
		{
			if (AActor* HitActor = Hit.GetActor())
			{
				if (ISelectable* Selectable = Cast<ISelectable>(HitActor))
				{
					return HitActor;
				}
			}
		}
	}
	return nullptr;
}

void APlayerPawn::CameraBounds()
{
	float NewPitch = TargetRotation.Pitch;
	if (TargetRotation.Pitch < (RotatePitchMax * -1))
	{
		NewPitch = RotatePitchMax * -1;
	}
	else if (TargetRotation.Pitch > (RotatePitchMin * -1))
	{
		NewPitch = RotatePitchMin * -1;
	}

	// Set new pitch and clamp any roll
	TargetRotation = FRotator(NewPitch, TargetRotation.Yaw, 0.f);
}

void APlayerPawn::GetTerrainPosition(FVector& TerrainPosition) const
{
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	FVector TraceOrigin = TerrainPosition;
	TraceOrigin.Z += 10000.f;
	FVector TraceEnd = TerrainPosition;
	TraceEnd.Z -= 10000.f;

	if (UWorld* WorldContext = GetWorld())
	{
		if (WorldContext->LineTraceSingleByChannel(Hit, TraceOrigin, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams))
		{
			TerrainPosition = Hit.ImpactPoint;
		}
	}
}

FCommandData APlayerPawn::CreateCommandData(const ECommandType Type) const
{
	if (!SPlayerController) return FCommandData();

	FRotator CommandRotation = FRotator::ZeroRotator;
	const FVector CommandEndLocation = SPlayerController->GetMousePositionOnGround();

	if ((CommandEndLocation - CommandLocation).Length() > 100.f)
	{
		const FVector Direction = CommandEndLocation - CommandLocation;
		const float RotationAngle = FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));
		CommandRotation = FRotator(0.f, RotationAngle, 0.f);
	}

	return FCommandData(CommandLocation, CommandRotation, Type);
}

/** Enchanced Input*/
/** Default */
void APlayerPawn::Move(const FInputActionValue& Value)
{
	if (!SpringArmComponent) return;

	if (ensure(Value.GetValueType() == EInputActionValueType::Axis2D))
	{
		TargetLocation += SpringArmComponent->GetTargetRotation().RotateVector(Value.Get<FVector>()) * MoveSpeed;
		GetTerrainPosition(TargetLocation);
	}
}

void APlayerPawn::Look(const FInputActionValue& Value)
{
	if (ensure(Value.GetValueType() == EInputActionValueType::Axis1D))
	{
		const float NewPitch = Value.Get<float>() * RotateSpeed * 0.5f;
		TargetRotation = FRotator(TargetRotation.Pitch + NewPitch, TargetRotation.Yaw, 0.f);
	}
}

void APlayerPawn::Rotate(const FInputActionValue& Value)
{
	if (ensure(Value.GetValueType() == EInputActionValueType::Axis1D))
	{
		const float NewRotation = Value.Get<float>() * RotateSpeed;
		TargetRotation = FRotator(TargetRotation.Pitch, TargetRotation.Yaw + NewRotation, 0.f);
	}
}

void APlayerPawn::Zoom(const FInputActionValue& Value)
{
	if (ensure(Value.GetValueType() == EInputActionValueType::Axis1D))
	{
		TargetZoom = FMath::Clamp(TargetZoom + (Value.Get<float>() * ZoomSpeed), MinZoom, MaxZoom);
	}
}

void APlayerPawn::Select(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	SPlayerController->Handle_Selection(nullptr);
	BoxSelect = false;
	SelectHitLocation = SPlayerController->GetMousePositionOnGround();
}

void APlayerPawn::SelectHold(const FInputActionValue& Value)
{
	if ((SelectHitLocation - SPlayerController->GetMousePositionOnGround()).Length() > 100.f)
	{
		if (!BoxSelect && SelectionBox)
		{
			SelectionBox->Start(SelectHitLocation, TargetRotation);
			BoxSelect = true;
		}
	}
}

void APlayerPawn::SelectEnd(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	if (BoxSelect && SelectionBox)
	{
		SelectionBox->End();
		BoxSelect = false;
	}
	else
	{
		SPlayerController->Handle_Selection(GetSelectedObject());
	}
}

void APlayerPawn::CommandStart(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	CommandLocation = SPlayerController->GetMousePositionOnGround();
	bIsCommanding = true;
	SPlayerController->DisplaySuggestedCommand(CreateCommandData(ECommandType::CommandMove));
}

void APlayerPawn::Command(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	SPlayerController->CommandSelected(CreateCommandData(ECommandType::CommandMove));
	bIsCommanding = false;
}

void APlayerPawn::SelectDoubleTap(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	if (AActor* Selection = GetSelectedObject())
	{
		if (AOrthoPvpStrategyCharacter* SelectedCharacter = Cast<AOrthoPvpStrategyCharacter>(Selection))
		{
			SPlayerController->Handle_DeSelection(SelectedCharacter);
			SelectedCharacter->Destroy();
		}
	}
}

void APlayerPawn::TestPlacement(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	SPlayerController->SetPlacementPreview();
}

/** Placement */

void APlayerPawn::Place(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	if (SPlayerController->IsPlacementModeEnabled())
	{
		SPlayerController->Place();
	}
}

void APlayerPawn::PlaceCancel(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	if (SPlayerController->IsPlacementModeEnabled())
	{
		SPlayerController->PlaceCancel();
	}
}

/** BuildMode */

void APlayerPawn::BuildDeploy(const FInputActionValue& Value)
{
	if (!BuildComponent) return;

	BuildComponent->BuildDeploy();
}

void APlayerPawn::BuildCancel(const FInputActionValue& Value)
{
	if (!BuildComponent) return;

	BuildComponent->ExitBuildMode();
}

/** Shift */

void APlayerPawn::Shift(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	SPlayerController->SetInputShift(Value.Get<bool>());
}

void APlayerPawn::ShiftSelect(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	if (AActor* Selection = GetSelectedObject())
	{
		const TSubclassOf<AActor> SelectionClass = Selection->GetClass();

		TArray<AActor*> Actors;
		Actors.Add(Selection);

		for (TActorIterator<AActor> ActorItr(GetWorld(), SelectionClass); ActorItr; ++ActorItr)
		{
			AActor* Actor = *ActorItr;
			const float DistanceSquared = FVector::DistSquared(Actor->GetActorLocation(), SelectHitLocation);
			if (DistanceSquared <= FMath::Square(1000.f))
			{
				Actors.AddUnique(Actor);
			}
		}

		SPlayerController->Handle_Selection(Actors);
	}
	else
	{
		SPlayerController->Handle_Selection(nullptr);
	}
}

void APlayerPawn::ShiftCommand(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	SPlayerController->CommandSelected(CreateCommandData(ECommandType::CommandMoveFast));
}
/** Alt */

void APlayerPawn::Alt(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	SPlayerController->SetInputAlt(Value.Get<bool>());
}

void APlayerPawn::AltSelect(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	BoxSelect = false;
	SelectHitLocation = SPlayerController->GetMousePositionOnGround();
}

void APlayerPawn::AltSelectEnd(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	if (BoxSelect && SelectionBox)
	{
		SelectionBox->End(false);
		BoxSelect = false;
	}
	else
	{
		SPlayerController->Handle_DeSelection(GetSelectedObject());
	}
}

void APlayerPawn::AltCommand(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	SPlayerController->CommandSelected(CreateCommandData(ECommandType::CommandMoveSlow));
}

/** Ctrl */

void APlayerPawn::Ctrl(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	SPlayerController->SetInputCtrl(Value.Get<bool>());
}

void APlayerPawn::CtrlSelect(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	BoxSelect = false;
	SelectHitLocation = SPlayerController->GetMousePositionOnGround();
}

void APlayerPawn::CtrlSelectEnd(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	if (BoxSelect && SelectionBox)
	{
		SelectionBox->End(true, true);
		BoxSelect = false;
	}
	else
	{
		SPlayerController->Handle_Selection(GetSelectedObject());
	}
}

void APlayerPawn::CtrlCommand(const FInputActionValue& Value)
{
	if (!SPlayerController) return;

	SPlayerController->CommandSelected(CreateCommandData(ECommandType::CommandMoveAttack));
}
