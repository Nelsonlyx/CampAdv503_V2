// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MotionControllerComponent.h"

#include "HandController.generated.h"


UCLASS()
class CAMPADV503_API AHandController : public AActor
{
	GENERATED_BODY()
	
public:	

	AHandController();

	void SetHand(EControllerHand Hand) { MotionController->SetTrackingSource(Hand); }
	void PairController(AHandController* Controller);

	void Grip();
	void Release();

	bool bIsZipButton = false;
	bool bIsZipButtonPressed = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Callbacks
	UFUNCTION()
	void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// Helpers
	bool CheckIfCanClimb() const;
	bool CheckIfZipGrabHandle() const;

	FVector RecordPosition(FVector CurrentLocation);
	FVector CalculateVelocity();


	// Default SubObject
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UMotionControllerComponent* MotionController;
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereCollider;
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Watch;

	UPROPERTY(EditDefaultsOnly)
	class UHapticFeedbackEffect_Base* HandHoldHaptic;

	// Config
	UPROPERTY(EditDefaultsOnly)
	float RightHPitchY = 70.0;
	UPROPERTY(EditDefaultsOnly)
	float RightHYawZ = 30;
	UPROPERTY(EditDefaultsOnly)
	float RightHRollX = 70;
	UPROPERTY(EditDefaultsOnly)
	float LeftHPitchY = -240;
	UPROPERTY(EditDefaultsOnly)
	float LeftHYawZ = -30;
	UPROPERTY(EditDefaultsOnly)
	float LefttHRollX =110;
	UPROPERTY(EditDefaultsOnly)
	FVector SphereTraceOffset = FVector(0.2, 0, 0);
	UPROPERTY(EditDefaultsOnly)
	float SphereTraceRadius = 15;
	UPROPERTY(EditDefaultsOnly)
	float VelocityLag = 0.95;


	// State
	bool bCanClimb = false;
	bool bNewCanClimb = false;
	bool bIsClimbing = false;
	FVector ClimbingStartLocation;
	FVector TrackedLocationPrevious;
	FVector TrackedLocationCurrent;
	FVector PlayerVelocity;


	AHandController* OtherController;
};
