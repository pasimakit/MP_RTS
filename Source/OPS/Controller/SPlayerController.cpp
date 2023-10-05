#include "SPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "EnhancedInputSubsystems.h"
#include "OPS/Interfaces/Selectable.h"
#include "OPS/Input/PlayerInputActions.h"
#include "OPS/Placement/PlacementPreview.h"
#include "OPS/OrthoPvpStrategyCharacter.h"
#include "OPS/Data/FormationDataAsset.h"
#include "Engine/AssetManager.h"
#include "OPS/Framework/SHUD.h"
#include "OPS/Build/SBuildComponent.h"

ASPlayerController::ASPlayerController(const FObjectInitializer& ObjectInitializer)
{
	bPlacementModeEnabled = false;
	BuildComponent = CreateDefaultSubobject<USBuildComponent>(TEXT("BuildComponent"));
}

void ASPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASPlayerController, Selected, COND_OwnerOnly);
}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	verify((AssetManager = UAssetManager::GetIfInitialized()) != nullptr);

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;

	CreateFormationData();

	if (BuildComponent)
	{
		BuildComponent->LoadBuildData();
	}

	if (ASHUD* HUD = Cast<ASHUD>(GetHUD()))
	{
		HUD->CreateHUD();
	}
}

void ASPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPlacementModeEnabled && PlacementPreviewActor)
	{
		UpdatePlacement();
	}
}

void ASPlayerController::Handle_Selection(AActor* ActorToSelect)
{
	if (ISelectable* Selectable = Cast<ISelectable>(ActorToSelect))
	{
		if (ActorToSelect && ActorSelected(ActorToSelect))
		{
			Server_DeSelect(ActorToSelect);
		}
		else
		{
			Server_Select(ActorToSelect);
		}
	}
	else
	{
		Server_ClearSelected();
	}
}

void ASPlayerController::Handle_Selection(TArray<AActor*> ActorsToDeSelect)
{
	Server_Select_Group(ActorsToDeSelect);
}

void ASPlayerController::Handle_DeSelection(AActor* ActorToDeSelect)
{
	if (ActorToDeSelect && ActorSelected(ActorToDeSelect))
	{
		Server_DeSelect(ActorToDeSelect);
	}
}

void ASPlayerController::Handle_DeSelection(TArray<AActor*> ActorsToSelect)
{
	Server_DeSelect_Group(ActorsToSelect);
}

FVector ASPlayerController::GetMousePositionOnGround() const
{
	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByChannel(OutHit, WorldLocation, WorldLocation + (WorldDirection * 100000.f), ECollisionChannel::ECC_GameTraceChannel1))
	{
		if (OutHit.bBlockingHit)
		{
			return OutHit.Location;
		}
	}

	return FVector::ZeroVector;
}

FVector ASPlayerController::GetMousePositionOnVisibility() const
{
	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByChannel(OutHit, WorldLocation, WorldLocation + (WorldDirection * 100000.f), ECollisionChannel::ECC_Visibility))
	{
		if (OutHit.bBlockingHit)
		{
			return OutHit.Location;
		}
	}

	return FVector::ZeroVector;
}

void ASPlayerController::Handle_MouseOver(AActor* ActorToMouseOver, bool Highlight)
{
	if (ActorToMouseOver)
	{
		if (Selected.Contains(ActorToMouseOver)) return;
		if (ISelectable* Selectable = Cast<ISelectable>(ActorToMouseOver))
		{
			Selectable->Highlight(Highlight, EHighlightType::EHT_Hover);
		}
	}
}

bool ASPlayerController::ActorSelected(AActor* ActorToCheck) const
{
	if (ActorToCheck && Selected.Contains(ActorToCheck))
	{
		return true;
	}
	return false;
}


void ASPlayerController::Server_Select_Implementation(AActor* ActorToSelect)
{
	if (ActorToSelect)
	{
		if (ISelectable* Selectable = Cast<ISelectable>(ActorToSelect))
		{
			Selectable->Select();
			Selected.Add(ActorToSelect);
			OnRep_Selected();
		}
	}
}

void ASPlayerController::Server_Select_Group_Implementation(const TArray<AActor*>& ActorsToSelect)
{
	TArray<AActor*> ValidActors;

	for (AActor* Actor : ActorsToSelect)
	{
		if (Actor)
		{
			if (ISelectable* Selectable = Cast<ISelectable>(Actor))
			{
				ValidActors.Add(Actor);
				Selectable->Select();
			}
		}
	}

	Selected.Append(ValidActors);
	OnRep_Selected();
	ValidActors.Empty();
}

void ASPlayerController::Server_DeSelect_Implementation(AActor* ActorToDeSelect)
{
	if (ActorToDeSelect)
	{
		if (ISelectable* Selectable = Cast<ISelectable>(ActorToDeSelect))
		{
			Selectable->DeSelect();
			Selected.Remove(ActorToDeSelect);
			OnRep_Selected();
		}
	}
}

void ASPlayerController::Server_ClearSelected_Implementation()
{
	for (auto Actor : Selected)
	{
		if (Actor)
		{
			if (ISelectable* Selectable = Cast<ISelectable>(Actor))
			{
				Selectable->DeSelect();
			}
		}
	}

	Selected.Empty();
	OnRep_Selected();
}

void ASPlayerController::Server_DeSelect_Group_Implementation(const TArray<AActor*>& ActorsToDeSelect)
{
	for (int i = 0; i < ActorsToDeSelect.Num(); ++i)
	{
		if (ActorsToDeSelect[i])
		{
			for (int j = Selected.Num() -1; j>=0; --j)
			{
				if (ActorsToDeSelect[i] == Selected[j])
				{
					if (ISelectable* Selectable = Cast<ISelectable>(ActorsToDeSelect[i]))
					{
						Selectable->DeSelect();
						Selected.RemoveAt(j);
						break;
					}
				}
			}
		}
	}
	
	OnRep_Selected();
}

void ASPlayerController::OnRep_Selected()
{
	OnSelectedUpdated.Broadcast();
}

/** Enchanced Input*/

void ASPlayerController::AddInputMapping(const UInputMappingContext* InputMapping, const int32 MappingPriority) const
{
	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		ensure(InputMapping);

		if (!InputSubsystem->HasMappingContext(InputMapping))
		{
			InputSubsystem->AddMappingContext(InputMapping, MappingPriority);
		}
	}
}

void ASPlayerController::RemoveInputMapping(const UInputMappingContext* InputMapping) const
{
	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		ensure(InputMapping);
		InputSubsystem->RemoveMappingContext(InputMapping);
	}
}

void ASPlayerController::SetInputDefault(const bool Enabled) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionsAsset is NULL"));

	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappignContextDefault);

		if (Enabled)
		{
			AddInputMapping(PlayerActions->MappignContextDefault, PlayerActions->MapPriorityDefault);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappignContextDefault);
		}
	}
}

void ASPlayerController::SetInputPlacement(const bool Enabled) const
{
	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappignContextPlacement);

		if(Enabled)
		{ 
			AddInputMapping(PlayerActions->MappignContextPlacement, PlayerActions->MapPriorityPlacement);
			SetInputDefault(!Enabled);		
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappignContextPlacement);
			SetInputDefault();
		}
	}
}

void ASPlayerController::SetInputBuildMode(const bool Enabled) const
{
	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappignContextBuildMode);

		if (Enabled)
		{
			AddInputMapping(PlayerActions->MappignContextBuildMode, PlayerActions->MapPriorityBuildMode);
			SetInputDefault(!Enabled);		
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappignContextBuildMode);
			SetInputDefault();
		}
	}
}

void ASPlayerController::SetInputShift(const bool Enabled) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionsAsset is NULL"));

	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappignContextShift);

		if (Enabled)
		{
			AddInputMapping(PlayerActions->MappignContextShift, PlayerActions->MapPriorityShift);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappignContextShift);
		}
	}
}

void ASPlayerController::SetInputAlt(const bool Enabled) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionsAsset is NULL"));

	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappignContextAlt);

		if (Enabled)
		{
			AddInputMapping(PlayerActions->MappignContextAlt, PlayerActions->MapPriorityAlt);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappignContextAlt);
		}
	}
}

void ASPlayerController::SetInputCtrl(const bool Enabled) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionsAsset is NULL"));

	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappignContextCtrl);

		if (Enabled)
		{
			AddInputMapping(PlayerActions->MappignContextCtrl, PlayerActions->MapPriorityCtrl);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappignContextCtrl);
		}
	}
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		InputSubsystem->ClearAllMappings();
		SetInputDefault();
	}
}

void ASPlayerController::SetPlacementPreview()
{
	if (PreviewActorType && !bPlacementModeEnabled)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetMousePositionOnVisibility());
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		PlacementPreviewActor = GetWorld()->SpawnActor<APlacementPreview>(PreviewActorType, SpawnTransform, SpawnParams);

		if (PlacementPreviewActor)
		{
			SetInputPlacement();

			bPlacementModeEnabled = true;
		}
	}
}

void ASPlayerController::UpdatePlacement() const
{
	if (!PlacementPreviewActor) return;

	PlacementPreviewActor->SetActorLocation(GetMousePositionOnVisibility());
}

void ASPlayerController::Place()
{
	if (!IsPlacementModeEnabled() || !PlacementPreviewActor) return;

	bPlacementModeEnabled = false;

	SetInputPlacement(false);

	Server_Place(PlacementPreviewActor);
}

void ASPlayerController::Server_Place_Implementation(AActor* PlacementPreviewToSpawn)
{
	if (const APlacementPreview* Preview = Cast<APlacementPreview>(PlacementPreviewToSpawn))
	{
		FTransform SpawnTransform;
		FVector Location = Preview->GetActorLocation();
		SpawnTransform.SetLocation(FVector(Location.X, Location.Y, Location.Z + 100.f));
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (AOrthoPvpStrategyCharacter* NewUnit = GetWorld()->SpawnActor<AOrthoPvpStrategyCharacter>(Preview->PlaceableClass, SpawnTransform, SpawnParams))
		{
			NewUnit->SetOwner(this);
		}
	}

	EndPlacement();
}

void ASPlayerController::EndPlacement_Implementation()
{
	PlacementPreviewActor->Destroy();
}

void ASPlayerController::PlaceCancel()
{
	if (!IsPlacementModeEnabled() || !PlacementPreviewActor) return;

	bPlacementModeEnabled = false;

	SetInputPlacement(false);

	EndPlacement();
}

void ASPlayerController::CommandSelected(FCommandData CommandData)
{
	Server_CommandSelected(CommandData);
}

void ASPlayerController::CreateFormationData()
{
	const FPrimaryAssetType AssetType("FormationData");
	TArray<FPrimaryAssetId> Formations;
	AssetManager->GetPrimaryAssetIdList(AssetType, Formations);

	if (Formations.Num() > 0)
	{
		const TArray<FName> Bundles;
		const FStreamableDelegate FormationDataLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ASPlayerController::OnFormationDataLoaded, Formations);
		AssetManager->LoadPrimaryAssets(Formations, Bundles, FormationDataLoadedDelegate);
	}
}

void ASPlayerController::OnFormationDataLoaded(TArray<FPrimaryAssetId> Formations)
{
	for (int i = 0; i < Formations.Num(); ++i)
	{
		if (UFormationDataAsset* FormationDataAsset = Cast<UFormationDataAsset>(AssetManager->GetPrimaryAssetObject(Formations[i])))
		{
			FormationData.Add(FormationDataAsset);
		}
	}
}

UFormationDataAsset* ASPlayerController::GetFormationData()
{
	for (int i = 0; i < FormationData.Num(); ++i)
	{
		if (FormationData[i]->FormationType == CurrentFormation)
		{
			return FormationData[i];
		}
	}

	return nullptr;
}

void ASPlayerController::CalculateOffset(const int Index, FCommandData& CommandData)
{
	if (FormationData.Num() <= 0) return;

	if (const UFormationDataAsset* CurrentFormationData = GetFormationData())
	{
		FVector Offset = CurrentFormationData->Offset;

		switch (CurrentFormationData->FormationType)
		{
			case EFormation::Blob:
			{
				if (Index != 0)
				{
					float Angle = (Index / static_cast<float>(Selected.Num())) * 2 * PI;

					float MinSpacing = FormationSpacing * 0.5f;
					if (Index % 2 == 0)
					{
						MinSpacing = MinSpacing * -1;
					}
					const float Radius = FMath::RandRange(MinSpacing, FormationSpacing);

					Offset.X = Radius * FMath::Cos(Angle);
					Offset.Y = Radius * FMath::Sin(Angle);
				}

				break;
			}
			default:
			{
				if (CurrentFormationData->Alternate)
				{
					if (Index % 2 == 0)
					{
						Offset.Y = Offset.Y * -1;
					}

					Offset *= (FMath::Floor((Index + 1) / 2)) * FormationSpacing;
				}
				else
				{
					Offset *= Index * FormationSpacing;
				}
			}
		}

		Offset = CommandData.Rotation.RotateVector(Offset);
		CommandData.Location = CommandData.SourceLocation + Offset;
	}
}

void ASPlayerController::UpdateFormation(const EFormation Formation)
{
	CurrentFormation = Formation;

	if (HasGroupSelection() && Selected.IsValidIndex(0))
	{
		CommandSelected(FCommandData(Selected[0]->GetActorLocation(), Selected[0]->GetActorRotation(), ECommandType::CommandMove));
	}
}

void ASPlayerController::DisplaySuggestedCommand(FCommandData CommandData)
{
	for (int i = 0; i < Selected.Num(); ++i)
	{
		if (AOrthoPvpStrategyCharacter* SelectedActor = Cast<AOrthoPvpStrategyCharacter>(Selected[i]))
		{
			CalculateOffset(i, CommandData);
			SelectedActor->SetToMoveMarker(CommandData.Location);
		}
	}
}

void ASPlayerController::UpdateSpacing(const float NewFormationSpacing)
{
	FormationSpacing = NewFormationSpacing;

	if (HasGroupSelection() && Selected.IsValidIndex(0))
	{
		CommandSelected(FCommandData(Selected[0]->GetActorLocation(), Selected[0]->GetActorRotation(), ECommandType::CommandMove));
	}
}

void ASPlayerController::Server_CommandSelected_Implementation(FCommandData CommandData)
{
	if (!HasAuthority()) return;

	for (int i = 0; i < Selected.Num(); ++i)
	{
		if (AOrthoPvpStrategyCharacter* SelectedActor = Cast<AOrthoPvpStrategyCharacter>(Selected[i]))
		{
			CalculateOffset(i, CommandData);
			SelectedActor->CommandMoveToLocation(CommandData);
		}
	}
}
