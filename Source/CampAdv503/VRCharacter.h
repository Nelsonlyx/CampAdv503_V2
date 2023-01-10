// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HandController.h"
#include "InputActionValue.h"

#include "VRCharacter.generated.h"

UCLASS()
class CAMPADV503_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRCharacter();

protected:
	virtual void BeginPlay() override;

	// Interactions
	void MoveForward(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);


public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* Camera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputMappingContext* DefaultMappingContext;


	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* VRMoveForward;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* VRMoveRight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* GripLeftHand;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* GripRightHand;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* FacebuttonA;

	// Config
	UPROPERTY(EditAnywhere)
	float MovementSpeed = 2;
	UPROPERTY(EditAnywhere)
	float TeleportFadeDuration = 0.3f;

	// State
	bool bIsClimbing = false;


	void EnableZipline(bool bActorInOverlap, class AZipline* OverlappedZipline, FVector ZipStartLoc, FVector ZipEndLoc);
	void StartFade(float FromAlpha, float ToAlpha);
	UFUNCTION(BlueprintImplementableEvent)
	void BeginZiplining();

private:

	void GripLeft() { LeftController->Grip(); }
	void ReleaseLeft() { LeftController->Release(); }
	void GripRight() { RightController->Grip(); }
	void ReleaseRight() { RightController->Release(); }

	void SetBlinkers();
	FVector2D GetBlinkerCenter();

private:

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class USceneComponent* VRRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class AHandController* RightController;
	UPROPERTY()
	class AHandController* LeftController;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHandController> HandControllerClass;

	// Walking Blinkers
	UPROPERTY(EditAnywhere)
	class UMaterialInterface* BlinkerMaterial;
	UPROPERTY(EditAnywhere)
	float BlinkerRadius = 0.5;
	UPROPERTY()
	UMaterialInstanceDynamic* BlinkerMatInstance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class UPostProcessComponent* PostProcessComponent;

	// Configuration Parameters
	UPROPERTY(EditAnywhere)
	class UCurveFloat* VelocityVsRadius;
	UPROPERTY()
	float PlayerSpeed;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bFreeLocomotion = true;

	// Zipline
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class AZipline* OverlappedZiplineclass;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bZiplineEnabled = false;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector ZiplineStart;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector ZiplineEnd;

	// SnapZone
	class APlayerSnapZone* SnapZone;

};
