#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SelectionBox.generated.h"

UCLASS()
class ORTHOPVPSTRATEGY_API ASelectionBox : public AActor
{
	GENERATED_BODY()
	
public:	
	ASelectionBox();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Start(FVector Position, const FRotator Rotation);

	UFUNCTION()
	void End(const bool bSelect = true, const bool bAddOnly = false);

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void Adjust() const;

	UFUNCTION()
	void Manage();

	UFUNCTION()
	void HandleHighlight(AActor* ActorInBox, const bool Highlight = true) const;

	UFUNCTION()
	void OnBoxColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
	FVector StartLocation;

	UPROPERTY()
	FRotator StartRotation;
	
	UPROPERTY()
	TArray<AActor*> InBox;

	UPROPERTY()
	TArray<AActor*> CenterInBox;

private:	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxCollider;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UDecalComponent* DecalComponent;

	UPROPERTY()
	bool BoxSelect;

	UPROPERTY()
	bool bDeselecting;

	UPROPERTY()
	class ASPlayerController* SPlayerController;
};
