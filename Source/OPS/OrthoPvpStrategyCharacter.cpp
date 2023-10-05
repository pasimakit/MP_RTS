// Copyright Epic Games, Inc. All Rights Reserved.

#include "OrthoPvpStrategyCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"
#include "AI/SAIController.h"
#include "Net/UnrealNetwork.h"
#include "Engine/AssetManager.h"
#include "OPS/Data/SCharacterData.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AOrthoPvpStrategyCharacter

AOrthoPvpStrategyCharacter::AOrthoPvpStrategyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	CharacterMovementComp = GetCharacterMovement();
	if (CharacterMovementComp)
	{
		CharacterMovementComp->bOrientRotationToMovement = true; // Character moves in the direction of input...	
		CharacterMovementComp->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
		MaxSpeed = CharacterMovementComp->MaxWalkSpeed;
	}

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AOrthoPvpStrategyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOrthoPvpStrategyCharacter, CharacterDataAssetId);
}

void AOrthoPvpStrategyCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();


	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	LoadData();
}

void AOrthoPvpStrategyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldOrientate)
	{
		SetOrientation(DeltaTime);

		if (IsOrientated())
		{
			bShouldOrientate = false;
		}
	}
}

FTransform AOrthoPvpStrategyCharacter::GetPositionTransform(const FVector Position) const
{
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	FVector TraceOrigin = Position;
	TraceOrigin.Z += 10000.f;
	FVector TraceEnd = Position;
	TraceEnd.Z -= 10000.f;

	if (UWorld* WorldContext = GetWorld())
	{
		if (WorldContext->LineTraceSingleByChannel(Hit, TraceOrigin, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams))
		{
			if (Hit.bBlockingHit)
			{
				FTransform HitTransform;
				HitTransform.SetLocation(Hit.ImpactPoint + FVector(1.f, 1.f, 3.5f));
				FRotator GroundRotation = UKismetMathLibrary::MakeRotFromZX(Hit.Normal, FVector::UpVector);
				GroundRotation += FRotator(90.f, 0.f, 0.f);
				HitTransform.SetRotation(GroundRotation.Quaternion());
				return HitTransform;
			}
		}
	}

	return FTransform::Identity;
}

void AOrthoPvpStrategyCharacter::Select()
{
	bSelected = true;
	Highlight(bSelected, EHighlightType::EHT_Select);
}

void AOrthoPvpStrategyCharacter::DeSelect()
{
	bSelected = false;
	Highlight(bSelected, EHighlightType::EHT_Select);
}

void AOrthoPvpStrategyCharacter::Highlight(const bool Highlight, const EHighlightType HighlightType)
{	
	float IntensityValue = .5f;
	switch (HighlightType)
	{
		case EHighlightType::EHT_Attack:
			break;
		case EHighlightType::EHT_Hover:
			IntensityValue = .5f;
			break;
		case EHighlightType::EHT_Select:
			IntensityValue = 5.f;
			break;
	}

	TArray<USkeletalMeshComponent*> Components;
	GetComponents<USkeletalMeshComponent>(Components);

	for (int32 i = 0; i < Components.Num(); i++)
	{
		if (Highlight)
		{
			if (UMaterialInstanceDynamic* HighlightMaterial = UMaterialInstanceDynamic::Create(GetHighlightMaterial(), this))
			{
				HighlightMaterial->SetScalarParameterValue("Glow", IntensityValue);
				Components[i]->SetOverlayMaterial(HighlightMaterial);
			}
		}
		else
		{
			Components[i]->SetOverlayMaterial(nullptr);
		}
	}
}



//////////////////////////////////////////////////////////////////////////
// Input

void AOrthoPvpStrategyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOrthoPvpStrategyCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOrthoPvpStrategyCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AOrthoPvpStrategyCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AOrthoPvpStrategyCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AOrthoPvpStrategyCharacter::CommandMoveToLocation(const FCommandData CommandData)
{
	switch (CommandData.Type)
	{
		case ECommandType::CommandMoveSlow:
		{
			SetWalk();
			break;
		}
		case ECommandType::CommandMoveFast:
		{
			SetSprint();
			break;
		}
		case ECommandType::CommandMoveAttack:
		{
		}
		default:
		{
			SetRun();
		}
	}

	CommandMove(CommandData);
}

void AOrthoPvpStrategyCharacter::LoadData() const
{
	if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		if (CharacterDataAssetId.IsValid())
		{
			AssetManager->LoadPrimaryAsset(CharacterDataAssetId);
		}
	}
}

USCharacterData* AOrthoPvpStrategyCharacter::GetCharacterData() const
{
	if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		if (CharacterDataAssetId.IsValid())
		{
			return Cast<USCharacterData>(AssetManager->GetPrimaryAssetObject(CharacterDataAssetId));
		}
	}

	return nullptr;
}

UMaterialInstance* AOrthoPvpStrategyCharacter::GetHighlightMaterial() const
{
	if (const USCharacterData* CharacterData = GetCharacterData())
	{
		const FSoftObjectPath AssetPath = CharacterData->HighlightMaterial.ToSoftObjectPath();
		return Cast<UMaterialInstance>(AssetPath.TryLoad());
	}

	return nullptr;
}

void AOrthoPvpStrategyCharacter::CommandMove(const FCommandData CommandData)
{
	if (SAIController)
	{
		SAIController->OnReachedDestination.Clear();
		SAIController->OnReachedDestination.AddDynamic(this, &AOrthoPvpStrategyCharacter::DestinationReached);
		SAIController->CommandMove(CommandData);
		SetMoveMarker(CommandData.Location);
	}
}

void AOrthoPvpStrategyCharacter::DestinationReached(const FCommandData CommandData)
{
	if (MoveMarker)
	{
		MoveMarker->Destroy();
	}
	TargetOrientation = CommandData.Rotation;
	bShouldOrientate = true;
}

void AOrthoPvpStrategyCharacter::SetWalk() const
{
	if (CharacterMovementComp)
	{
		CharacterMovementComp->MaxWalkSpeed = MaxSpeed * 0.5f;
	}
}

void AOrthoPvpStrategyCharacter::SetRun() const
{
	if (CharacterMovementComp)
	{
		CharacterMovementComp->MaxWalkSpeed = MaxSpeed;
	}
}

void AOrthoPvpStrategyCharacter::SetSprint() const
{
	if (CharacterMovementComp)
	{
		CharacterMovementComp->MaxWalkSpeed = MaxSpeed * 1.25f;
	}
}

void AOrthoPvpStrategyCharacter::SetOrientation(float DeltaTime)
{
	const FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw, 0.f), TargetOrientation, DeltaTime, 2.f);
	SetActorRotation(InterpolatedRotation);
}

bool AOrthoPvpStrategyCharacter::IsOrientated() const
{
	const FRotator CurrentRotation = GetActorRotation();
	return FMath::IsNearlyEqual(CurrentRotation.Yaw, TargetOrientation.Yaw, 0.25f);
}

void AOrthoPvpStrategyCharacter::SetMoveMarker(const FVector Location)
{
	if (MoveMarkerClass)
	{
		if (MoveMarker)
		{
			MoveMarker->Destroy();
		}
		if (ToMoveMarker)
		{
			ToMoveMarker->Destroy();
			ToMoveMarker = nullptr;
		}

		FActorSpawnParameters Params;
		Params.Instigator = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (UWorld* WorldContext = GetWorld())
		{
			MoveMarker = WorldContext->SpawnActor<AActor>(MoveMarkerClass, GetPositionTransform(Location), Params);
		}
	}
}

void AOrthoPvpStrategyCharacter::SetToMoveMarker(const FVector Location)
{
	if (ToMoveMarkerClass)
	{
		if (!ToMoveMarker)
		{
			FActorSpawnParameters Params;
			Params.Instigator = this;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			if (UWorld* WorldContext = GetWorld())
			{
				ToMoveMarker = WorldContext->SpawnActor<AActor>(ToMoveMarkerClass, GetPositionTransform(Location), Params);
			}
		}
		else
		{
			ToMoveMarker->SetActorLocation(Location);
		}
	}
}