// Fill out your copyright notice in the Description page of Project Settings.


#include "ZipButton.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "HandController.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AZipButton::AZipButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ButtonBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonBase"));
	SetRootComponent(ButtonBase);

	Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Button"));
	Button->SetupAttachment(ButtonBase);
}

// Called when the game starts or when spawned
void AZipButton::BeginPlay()
{
	Super::BeginPlay();

	Button->OnComponentBeginOverlap.AddDynamic(this, &AZipButton::ComponentBeginOverlap);
	Button->OnComponentEndOverlap.AddDynamic(this, &AZipButton::ComponentEndOverlap);

	ButtonInitialLocation = Button->GetComponentLocation();
}

// Called every frame
void AZipButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsTrackingPlayer && HandController != nullptr)
	{
		FVector HandDisplacement = HandController->GetActorLocation() - HandInitialLocation;
		FVector HandMovementAlongButton = UKismetMathLibrary::ProjectVectorOnToVector(HandDisplacement, Button->GetForwardVector());
		float HandMovementFloat = UKismetMathLibrary::Abs(HandMovementAlongButton.Length());
		ButtonTravelAmount = UKismetMathLibrary::Clamp(HandMovementFloat, 0, MaxButtonTravel);

		ButtonCurrentLocation = ButtonInitialLocation - (Button->GetForwardVector() * ButtonTravelAmount);
		Button->SetWorldLocation(ButtonCurrentLocation);

		if (ButtonTravelAmount >= MaxButtonTravel)
		{
			HandController->bIsZipButtonPressed = true;
			StopButtonTracking();
		}
	}
	else if (bIsButtonReturning)
	{
		ButtonCurrentLocation = UKismetMathLibrary::VInterpTo(ButtonCurrentLocation, ButtonInitialLocation, DeltaTime, ButtonReturnSpeed);
		Button->SetWorldLocation(ButtonCurrentLocation);

		if (ButtonCurrentLocation == ButtonInitialLocation)
		{
			bIsButtonReturning = false;
		}
	}
}

void AZipButton::ComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	SetActorTickEnabled(true);
	HandController = Cast<AHandController>(OtherActor);
	if (HandController != nullptr)
	{
		bIsTrackingPlayer = true;
		HandInitialLocation = HandController->GetActorLocation();
	}
}

void AZipButton::ComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	SetActorTickEnabled(false);
}

void AZipButton::StopButtonTracking()
{
	bIsTrackingPlayer = false;
	bIsButtonReturning = true;
}


