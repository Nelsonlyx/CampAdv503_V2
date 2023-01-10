// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/PostProcessComponent.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveFloat.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Math/Vector2D.h"
#include "Zipline.h"
#include "PlayerSnapZone.h"


// Sets default values
AVRCharacter::AVRCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));

}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (BlinkerMaterial != nullptr)
	{
		BlinkerMatInstance = UMaterialInstanceDynamic::Create(BlinkerMaterial, this);
		PostProcessComponent->AddOrUpdateBlendable(BlinkerMatInstance);
	}

	LeftController = GetWorld()->SpawnActor<AHandController>(HandControllerClass);
	if (LeftController != nullptr)
	{
		LeftController->AttachToComponent(VRRoot, FAttachmentTransformRules::KeepRelativeTransform);
		LeftController->SetHand(EControllerHand::Left);
		LeftController->SetOwner(this);
	}
	RightController = GetWorld()->SpawnActor<AHandController>(HandControllerClass);
	if (RightController != nullptr)
	{
		RightController->AttachToComponent(VRRoot, FAttachmentTransformRules::KeepRelativeTransform);
		RightController->SetHand(EControllerHand::Right);
		RightController->SetOwner(this);
	}

	LeftController->PairController(RightController);

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewCameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	NewCameraOffset.Z = 0;
	AddActorWorldOffset(NewCameraOffset);
	VRRoot->AddWorldOffset(-NewCameraOffset);

	SetBlinkers();
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		if (bFreeLocomotion)
		{
			EnhancedInputComponent->BindAction(VRMoveForward, ETriggerEvent::Triggered, this, &AVRCharacter::MoveForward);
			EnhancedInputComponent->BindAction(VRMoveRight, ETriggerEvent::Triggered, this, &AVRCharacter::MoveRight);
		}
		// Climbing
		EnhancedInputComponent->BindAction(GripLeftHand, ETriggerEvent::Started, this, &AVRCharacter::GripLeft);
		EnhancedInputComponent->BindAction(GripLeftHand, ETriggerEvent::Completed, this, &AVRCharacter::ReleaseLeft);
		EnhancedInputComponent->BindAction(GripRightHand, ETriggerEvent::Started, this, &AVRCharacter::GripRight);
		EnhancedInputComponent->BindAction(GripRightHand, ETriggerEvent::Completed, this, &AVRCharacter::ReleaseRight);
	}

}
void AVRCharacter::MoveForward(const FInputActionValue& Value)
{
	float Movement = Value.GetMagnitude() * MovementSpeed;

	AddMovementInput(Camera->GetForwardVector(), Movement);
}

void AVRCharacter::MoveRight(const FInputActionValue& Value)
{
	float Movement = Value.GetMagnitude() * MovementSpeed;

	AddMovementInput(Camera->GetRightVector(), Movement);
}


void AVRCharacter::EnableZipline(bool bActorInOverlap, class AZipline* OverlappedZipline, FVector ZipStartLoc, FVector ZipEndLoc)
{
	bZiplineEnabled = bActorInOverlap;
	OverlappedZiplineclass = OverlappedZipline;
	ZiplineStart = ZipStartLoc;
	ZiplineEnd = ZipEndLoc;
}


void AVRCharacter::StartFade(float FromAlpha, float ToAlpha)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC != nullptr)
	{
		PC->PlayerCameraManager->StartCameraFade(FromAlpha, ToAlpha, TeleportFadeDuration, FLinearColor::Black);
	}
}

void AVRCharacter::SetBlinkers()
{
	if (VelocityVsRadius == nullptr)
	{
		return;
	}
	PlayerSpeed = GetVelocity().Size();
	BlinkerRadius = VelocityVsRadius->GetFloatValue(PlayerSpeed);
	BlinkerMatInstance->SetScalarParameterValue(TEXT("Radius"), BlinkerRadius);

	FVector2D CenterPos = GetBlinkerCenter();
	BlinkerMatInstance->SetVectorParameterValue(TEXT("CenterPos"), FLinearColor(CenterPos.X, CenterPos.Y, 0, 0));
}

FVector2D AVRCharacter::GetBlinkerCenter()
{
	FVector MovementDirection = GetVelocity().GetSafeNormal();
	if (MovementDirection.IsNearlyZero())
	{
		return FVector2D(0.5, 0.5);
	}

	FVector WorldLocation;
	if (FVector::DotProduct(Camera->GetForwardVector(), MovementDirection) > 0)
	{
		WorldLocation = Camera->GetComponentLocation() + MovementDirection * 1000;
	}
	else
	{
		WorldLocation = Camera->GetComponentLocation() - MovementDirection * 1000;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC == nullptr)
	{
		return FVector2D(0.5, 0.5);
	}
	FVector2D ScreenLocation;
	PC->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation);

	int32 SizeX, SizeY;
	PC->GetViewportSize(SizeX, SizeY);
	ScreenLocation.X /= SizeX;
	ScreenLocation.Y /= SizeY;

	FVector2D ClampedScreenLocation = ScreenLocation.ClampAxes(0.4, 0.6);
	FString ScreenLocStr = ClampedScreenLocation.ToString();

	return ClampedScreenLocation;
}

