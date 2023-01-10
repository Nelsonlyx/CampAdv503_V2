// Fill out your copyright notice in the Description page of Project Settings.


#include "HandController.h"
#include "VRCharacter.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"


// Sets default values
AHandController::AHandController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	SetRootComponent(MotionController);

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->SetupAttachment(MotionController);

	Watch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Watch"));
	Watch->SetupAttachment(MotionController);
}

// Called when the game starts or when spawned
void AHandController::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AHandController::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AHandController::ActorEndOverlap);
}

// Called every frame
void AHandController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsClimbing)
	{
		RecordPosition(GetActorLocation());
		
		FVector HandMovement = GetActorLocation() - ClimbingStartLocation;
		GetAttachParentActor()->AddActorWorldOffset(-HandMovement);

		CalculateVelocity();
	}

	if (bIsZipButtonPressed)
	{
		AVRCharacter* VRChar = Cast<AVRCharacter>(GetAttachParentActor());
		if (VRChar != nullptr)
		{
			VRChar->BeginZiplining();
			bIsZipButtonPressed = false;
		}
	}
}

void AHandController::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	bNewCanClimb = CheckIfCanClimb();
	if (!bCanClimb && bNewCanClimb)
	{
		APawn* Pawn = Cast<APawn>(GetAttachParentActor());
		if (Pawn != nullptr)
		{
			APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
			if (PC != nullptr)
			{
				PC->PlayHapticEffect(HandHoldHaptic, MotionController->GetTrackingSource());
			}
		}
	}
	bCanClimb = bNewCanClimb;

}

void AHandController::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	bCanClimb = CheckIfCanClimb();
}

void AHandController::Grip()
{
	if (!bCanClimb) { return; }
	if (bCanClimb && !bIsClimbing)
	{

		bIsClimbing = true;
		ClimbingStartLocation = GetActorLocation();

		OtherController->bIsClimbing = false;

		AVRCharacter* VRChar = Cast<AVRCharacter>(GetAttachParentActor());
		if (VRChar != nullptr)
		{
			VRChar->bIsClimbing = true;
		}

		ACharacter* Character = Cast<ACharacter>(GetAttachParentActor());
		if (Character != nullptr)
		{
			Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		}
	}

}

void AHandController::Release()
{
	if (bIsClimbing)
	{
		bIsClimbing = false;

		AVRCharacter* VRChar = Cast<AVRCharacter>(GetAttachParentActor());
		if (VRChar != nullptr && !OtherController->bIsClimbing)
		{
			VRChar->bIsClimbing = false;
		}

		ACharacter* Character = Cast<ACharacter>(GetAttachParentActor());
		if (Character != nullptr)
		{
			Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
			Character->LaunchCharacter(-PlayerVelocity, false, false);
		}
	}

}


void AHandController::PairController(AHandController* Controller)
{
	OtherController = Controller;
	OtherController->OtherController = this;
}


bool AHandController::CheckIfCanClimb() const
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor->ActorHasTag(TEXT("Climbable")))
		{
			return true;
		}
	}
	return false;
}

FVector AHandController::RecordPosition(FVector CurrentLocation)
{
	ACharacter* Character = Cast<ACharacter>(GetAttachParentActor());
	if (Character != nullptr)
	{
		TrackedLocationPrevious = CurrentLocation;
		return TrackedLocationPrevious;
	}

	return FVector(0, 0, 0);
}

FVector AHandController::CalculateVelocity()
{
	TrackedLocationCurrent = GetActorLocation();

	FVector Displacement = TrackedLocationPrevious - TrackedLocationCurrent;
	PlayerVelocity = Displacement / GetWorld()->GetDeltaSeconds();
	return PlayerVelocity;
}

