#include "SelectionBox.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "OPS/Controller/SPlayerController.h"
#include "OPS/Interfaces/Selectable.h"

ASelectionBox::ASelectionBox()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetBoxExtent(FVector(1.0f, 1.0f, 1.0f));
	BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollider->SetCollisionResponseToAllChannels(ECR_Overlap);
	BoxCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ASelectionBox::OnBoxColliderBeginOverlap);
	RootComponent = BoxCollider;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetupAttachment(RootComponent);


	BoxSelect = false;
}



void ASelectionBox::BeginPlay()
{
	Super::BeginPlay();

	SetActorEnableCollision(false);
	if (DecalComponent)
	{
		DecalComponent->SetVisibility(false);
	}

	SPlayerController = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void ASelectionBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BoxSelect)
	{
		Adjust();
		Manage();
	}
}

void ASelectionBox::Start(FVector Position, const FRotator Rotation)
{
	if (!DecalComponent) return;
	
	StartLocation = FVector(Position.X, Position.Y, 0.f);
	StartRotation = FRotator(0.f, Rotation.Yaw, 0.f);

	SetActorLocation(StartLocation);
	SetActorRotation(StartRotation);
	SetActorEnableCollision(true);

	DecalComponent->SetVisibility(true);
	InBox.Empty();
	CenterInBox.Empty();
	BoxSelect = true;
}

void ASelectionBox::End(const bool bSelect, const bool bAddOnly)
{
	if (!SPlayerController) return;

	BoxSelect = false;
	SetActorEnableCollision(false);
	DecalComponent->SetVisibility(false);

	if (CenterInBox.Num() == 0)
	{
		if (!bAddOnly)
		{
			SPlayerController->Handle_Selection(nullptr);
		}
	}
	else
	{
		bSelect ? SPlayerController->Handle_Selection(CenterInBox) : SPlayerController->Handle_DeSelection(CenterInBox);
	}
	
	InBox.Empty();
	CenterInBox.Empty();
}

void ASelectionBox::Adjust() const
{
	if (!SPlayerController || !BoxCollider || !DecalComponent) return;

	const FVector CurrentMouseLocOnGround = SPlayerController->GetMousePositionOnGround();
	const FVector EndPoint = FVector(CurrentMouseLocOnGround.X, CurrentMouseLocOnGround.Y, 0.f);
	BoxCollider->SetWorldLocation(UKismetMathLibrary::VLerp(StartLocation, EndPoint, 0.5f));

	FVector NewExtent = FVector(GetActorLocation().X, GetActorLocation().Y, 0.f) - EndPoint;
	NewExtent = GetActorRotation().GetInverse().RotateVector(NewExtent);
	NewExtent = NewExtent.GetAbs();
	NewExtent.Z += 10000.f;
	BoxCollider->SetBoxExtent(NewExtent);

	FVector DecalSize;
	DecalSize.X = NewExtent.Z;
	DecalSize.Y = NewExtent.Y;
	DecalSize.Z = NewExtent.X;
	DecalComponent->DecalSize = DecalSize;
}

void ASelectionBox::Manage()
{
	if (!BoxCollider) return;

	for (AActor* Actor : InBox)
	{
		if (Actor)
		{
			FVector ActorCenter = Actor->GetActorLocation();
			const FVector LocalActorCenter = BoxCollider->GetComponentTransform().InverseTransformPosition(ActorCenter);
			const FVector LocalExtents = BoxCollider->GetUnscaledBoxExtent();

			// Check if actors center position is inside selection box
			if (LocalActorCenter.X >= -LocalExtents.X && LocalActorCenter.X <= LocalExtents.X
				&& LocalActorCenter.Y >= -LocalExtents.Y && LocalActorCenter.Y <= LocalExtents.Y
				&& LocalActorCenter.Z >= -LocalExtents.Z && LocalActorCenter.Z <= LocalExtents.Z)
			{
				if (!CenterInBox.Contains(Actor))
				{
					CenterInBox.Add(Actor);
					HandleHighlight(Actor, true);
				}
			}
			else
			{
				if (CenterInBox.Contains(Actor))
				{
					CenterInBox.Remove(Actor);
					HandleHighlight(Actor, false);
				}
			}
		}
	}
}

void ASelectionBox::HandleHighlight(AActor* ActorInBox, const bool Highlight) const
{
	if (ISelectable* Selectable = Cast<ISelectable>(ActorInBox))
	{
		Selectable->Highlight(Highlight, EHighlightType::EHT_Select);
	}
}

void ASelectionBox::OnBoxColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;

	if (ISelectable* Selectable = Cast<ISelectable>(OtherActor))
	{
		InBox.AddUnique(OtherActor);
	}
}

