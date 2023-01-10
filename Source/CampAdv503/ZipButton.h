// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZipButton.generated.h"

UCLASS()
class CAMPADV503_API AZipButton : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZipButton();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* ButtonBase;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* Button;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayerPressedButton;


	UFUNCTION(BlueprintImplementableEvent)
	void ChangeButtonColor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void ComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void ComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void StopButtonTracking();

	// State
	UPROPERTY()
	bool bIsTrackingPlayer = false;
	bool bIsButtonReturning = false;
	FVector HandInitialLocation;
	FVector ButtonInitialLocation;
	float ButtonTravelAmount;
	FVector ButtonCurrentLocation;

	// Config
	UPROPERTY(EditDefaultsOnly)
	int32 ButtonThreshold = 2;
	UPROPERTY(EditDefaultsOnly)
	int32 MaxButtonTravel = 3; 
	UPROPERTY(EditDefaultsOnly)
	float ButtonReturnSpeed = 2;

	// References
	class AHandController* HandController;

};
